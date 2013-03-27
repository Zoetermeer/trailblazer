#include "chunk.hpp"
#include "matrix-stack.hpp"
#include "shader.hpp"
#include "GL.hpp"
#include "sky.hpp"
#include "units.hpp"
#include <noise.h>
#include <chrono>

glm::ivec3 Chunk::worldToChunkSpace(glm::vec3 wc)
{
  const GLfloat FACTOR = Units::chunkToVoxel(1) * Units::voxelToGl(1);
  glm::vec3 v(wc.x / FACTOR, 0, -(wc.z / FACTOR));
  
  return glm::ivec3(ceil(v.x) - 1, v.y, ceil(v.z) - 1);
}

glm::ivec3 Chunk::getMinimumAbsVoxelCoord() const
{
  const unsigned VOXELS_PER_CHUNK = Units::chunkToVoxel(1);
  return glm::ivec3(m_chunkIndex.x * VOXELS_PER_CHUNK,
                    m_chunkIndex.y * VOXELS_PER_CHUNK,
                    m_chunkIndex.z * VOXELS_PER_CHUNK);
}

//This function should be thread-safe
bool Chunk::isVoxelActiveAt(voxel_coord_type x, voxel_coord_type y, voxel_coord_type z)
{
  voxel_key_type key = hashCoords(x, y, z);
  std::map<voxel_key_type, bool>::iterator it = m_voxelMap.find(key);
  return it != m_voxelMap.end();
}

GLclampf Chunk::accessibilityAt(voxel_coord_type x, voxel_coord_type y, voxel_coord_type z)
{
  const unsigned VOXELS_PER_CHUNK = Units::chunkToVoxel(1);
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

glm::vec3 Chunk::getNoiseModuleInput(const glm::vec3 &voxelRelativeCoords)
{
  const float INTERP = 1.f / (float)Units::chunkToVoxel(1);
  return glm::vec3(m_chunkIndex.x + (voxelRelativeCoords.x * INTERP),
                   m_chunkIndex.y + (voxelRelativeCoords.y * INTERP),
                   m_chunkIndex.z + (voxelRelativeCoords.z * INTERP));
}

float Chunk::noiseAt(int x, int z)
{
  return m_noiseModule->GetValue(x, 0.0, z);
}

GLfloat Chunk::heightAt(int x, int z)
{
  const float INTERP = 1 / Units::chunkToVoxel(1);
  float inx = m_chunkIndex.x + (x * INTERP);
  float inz = m_chunkIndex.z + (z * INTERP);
  
  glm::vec3 input = getNoiseModuleInput(glm::vec3(x, 1, z));
  float noise = m_noiseModule->GetValue(input.x, input.y, input.z);
  
  return Units::noiseToFeet(noise);
}

//Generate the height map and stash
void Chunk::generateData()
{
  auto mountain = new noise::module::RidgedMulti();
  mountain->SetOctaveCount(9);
  mountain->SetFrequency(0.2);
  auto baseFlat = new noise::module::Billow();
  auto flatTerrain = new noise::module::ScaleBias();
  flatTerrain->SetSourceModule(0, *baseFlat);
  flatTerrain->SetScale(0.125);
  flatTerrain->SetBias(-0.96);
  
  //Perlin to control which type of terrain to generate
  auto terrainType = new noise::module::Perlin();
  terrainType->SetOctaveCount(4);
  terrainType->SetFrequency(0.5);
  terrainType->SetPersistence(0.25);
  
  auto select = new noise::module::Select();
  m_noiseModule = select;
  select->SetSourceModule(0, *flatTerrain);
  select->SetSourceModule(1, *mountain);
  select->SetControlModule(*terrainType);
  select->SetBounds(0.0, 1000.0);
  select->SetEdgeFalloff(0.6);
  
  m_modules.push_back(mountain);
  m_modules.push_back(baseFlat);
  m_modules.push_back(flatTerrain);
  m_modules.push_back(terrainType);
  
//  auto perlin = new noise::module::Perlin();
//  perlin->SetOctaveCount(10);
//  perlin->SetPersistence(0.5);
//  perlin->SetFrequency(1.0);
//  //perlin->SetOctaveCount(3);
//  //perlin->SetFrequency(0.03);
//  //perlin->SetPersistence(0.25);
//  
//  auto billow = new noise::module::Billow();
//  billow->SetOctaveCount(1);
//  
//  auto mountains = new noise::module::RidgedMulti();
//  mountains->SetOctaveCount(8);
//  
//  auto terrainType = new noise::module::Perlin();
//  auto selector = new noise::module::Select();
//  m_noiseModule = perlin;
//  selector->SetSourceModule(0, *perlin);
//  selector->SetSourceModule(1, *billow);
//  selector->SetControlModule(*terrainType);
//
//  m_modules.push_back(perlin);
//  m_modules.push_back(billow);
//  m_modules.push_back(terrainType);
//  m_modules.push_back(selector);
}

void Chunk::generateBuffers()
{
  if (m_vertexBuffer)
    m_vertexBuffer->end();
  
  m_voxelMap.clear();
}

void Chunk::doGenerate(Chunk *chunk)
{
  chunk->generateData();
}

void Chunk::generate()
{
  if (m_generated)
    return;
  
  generateData();
  generateBuffers();
  m_generated = true;
}

bool Chunk::generateAsync()
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
  generateBuffers();
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
  const unsigned VOXELS_PER_CHUNK = Units::chunkToVoxel(1);
  debug_stats &stats = env.getStats();
  stats.vertices += m_vertexCount;
  stats.voxels += VOXELS_PER_CHUNK * VOXELS_PER_CHUNK * VOXELS_PER_CHUNK;
  stats.active_voxels += m_activeVoxels;
  stats.drawn_voxels += m_drawnVoxels;
  
  MatrixStack &mv = env.getMV();
  ShaderSet &shaders = env.getShaders();
  GLfloat offset = VOXELS_PER_CHUNK * Units::voxelToGl(1);
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








