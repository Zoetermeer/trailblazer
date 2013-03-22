#include "chunk.hpp"
#include "matrix-stack.hpp"
#include "shader.hpp"
#include "GL.hpp"
#include "sky.hpp"
#include <noise.h>
#include <chrono>

//This function should be thread-safe
bool Chunk::isVoxelActiveAt(voxel_coord_type x, voxel_coord_type y, voxel_coord_type z)
{
  voxel_key_type key = hashCoords(x, y, z);
  std::map<voxel_key_type, bool>::iterator it = m_voxelMap.find(key);
  return it != m_voxelMap.end();
}

GLclampf Chunk::accessibilityAt(voxel_coord_type x, voxel_coord_type y, voxel_coord_type z)
{
  if (x < 0 || x > VOXELS_PER_CHUNK - 1)
    return 1.f;
  if (y < 0 || y > VOXELS_PER_CHUNK - 1)
    return 1.f;
  if (z < 0 || z > VOXELS_PER_CHUNK - 1)
    return 1.f;
  
  if (isVoxelActiveAt(x, y, z))
    return .9f;
  
  return 1.f;
}

//Hash using a z-order curve, where we interleave the bits of each
//coordinate value
voxel_key_type Chunk::hashCoords(voxel_coord_type x, voxel_coord_type y, voxel_coord_type z)
{
  size_t coordBitSz = sizeof(voxel_coord_type) * 8;
  voxel_key_type hashed = 0;
  int hashPos = coordBitSz * 3 - 1;
  voxel_coord_type testVal;
  for (int pos = coordBitSz - 1; pos >= 0; pos--) {
    testVal = 1 << pos;
    if ((x & testVal) > 0)
      hashed |= 1 << hashPos;
    
    hashPos--;
    if ((y & testVal) > 0)
      hashed |= 1 << hashPos;
    
    hashPos--;
    if ((z & testVal) > 0)
      hashed |= 1 << hashPos;
    
    hashPos--;
  }
  
  return hashed;
}

GLfloat Chunk::heightAt(int x, int z)
{
  GLfloat noise = m_heightMap.GetValue(x, z);
  noise += 1.f;
  return noise * VOXELS_PER_CHUNK * .04;
//  int ht = (VOXELS_PER_CHUNK * .5) * noise;
//  ht += VOXELS_PER_CHUNK * .5;
//  if (!ht) ht = 1;
//  
//  return ht;
  return noise;
}

//Generate the height map and stash
void Chunk::generateData()
{
  noise::module::RidgedMulti mountain;
  noise::module::Billow baseFlat;
  baseFlat.SetFrequency(2.0);
  noise::module::ScaleBias flatTerrain;
  flatTerrain.SetSourceModule(0, baseFlat);
  flatTerrain.SetScale(0.125);
  flatTerrain.SetBias(-0.75);
  
  //Perlin to control which type of terrain to generate
  noise::module::Perlin terrainType;
  terrainType.SetOctaveCount(1);
  terrainType.SetFrequency(0.2);
  terrainType.SetPersistence(0.25);
  
  noise::module::Select terrainSelector;
  terrainSelector.SetSourceModule(0, flatTerrain);
  terrainSelector.SetSourceModule(1, mountain);
  terrainSelector.SetControlModule(terrainType);
  terrainSelector.SetBounds(0.0, 1000.0);
  terrainSelector.SetEdgeFalloff(0.6);
  
  utils::NoiseMapBuilderPlane heightMapBuilder;
  heightMapBuilder.SetSourceModule(terrainSelector);
  heightMapBuilder.SetDestNoiseMap(m_heightMap);
  heightMapBuilder.SetDestSize(VOXELS_PER_CHUNK + 1, VOXELS_PER_CHUNK + 1);
  heightMapBuilder.SetBounds(m_chunkIndex.x,
                             m_chunkIndex.x + 1,
                             m_chunkIndex.z,
                             m_chunkIndex.z + 1);
  heightMapBuilder.Build();
}

void Chunk::doGenerate(Chunk *chunk)
{
  chunk->generateData();
}

bool Chunk::generateDataAsync()
{
  if (m_generated)
    return true;
  
  if (!m_generatingAsync) {
    m_generatingAsync = true;
    m_generatingFuture = std::async(std::launch::async, Chunk::doGenerate, this);
    return false;
  }
  
  //Otherwise, check the status of the future
  std::future_status status = m_generatingFuture.wait_for(std::chrono::seconds(0));
  if (status != std::future_status::ready)
    return false;
  
  //Async work finished, generate the vertex buffer
  m_generatingAsync = false;
  m_vertexBuffer->end();
  m_voxelMap.clear();
  m_generated = true;
  return true;
}

void Chunk::draw(Env &env,
                 const glm::vec4 &playerPos,
                 const glm::vec3 &playerLookVec,
                 bool isHeadlightOn,
                 bool exploding,
                 GLclampf explosionTime)
{
  if (!isGenerated())
    return;
  
  //Update environment stats
  debug_stats &stats = env.getStats();
  stats.vertices += m_vertexCount;
  stats.voxels += VOXELS_PER_CHUNK * VOXELS_PER_CHUNK * VOXELS_PER_CHUNK;
  stats.active_voxels += m_activeVoxels;
  stats.drawn_voxels += m_drawnVoxels;
  
  MatrixStack &mv = env.getMV();
  ShaderSet &shaders = env.getShaders();
  GLfloat offset = VOXELS_PER_CHUNK * VOXEL_SIZE;
  mv.pushMatrix();
  {
    mv.translate(m_chunkIndex.x * offset, 0.f, m_chunkIndex.z * offset);
    glm::vec4 groundColor = m_containsPlayer ? glm::vec4(0.0f, 0.0f, 0.3f, 1.f) : GL::color(51, 102, 51);
    shaders.prepareHemisphereAO(env,
                                glm::vec4(.8f, .8f, .8f, 1.f),
                                groundColor,
                                exploding,
                                explosionTime);
    getVertexBuffer()->draw(GL_TRIANGLES);
  }
  mv.popMatrix();
}








