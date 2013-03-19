#include "chunk.hpp"
#include "matrix-stack.hpp"
#include "shader.hpp"
#include "GL.hpp"
#include "sky.hpp"
#include <noise.h>
#include "noiseutils.h"
#include <chrono>

#define VERT(m,x,y,z) (new_vertex((m * glm::vec4(x,y,z,1.f))))
#define NORMAL(a,b,c) (glm::normalize(glm::cross(glm::vec3(b.position - a.position), glm::vec3(c.position - a.position))))
void Chunk::addVoxel(const glm::vec3 &ind,
                     const Neighbors &ns,
                     const TerrainType &type,
                     VertexBatch *batch,
                     MatrixStack &stack)
{  
  //Don't bother with totally occluded voxels
  if (ns == Neighbors::All)
    return;
  
  //Stat-keeping
  m_drawnVoxels++;
  
  glm::mat4 &m = stack.current();
  glm::vec4 color;
  switch (type)
  {
    case TerrainType::Grass:
      color = GL::color(0, 100, 0);
      break;
    case TerrainType::Dirt:
      color = GL::color(100, 69, 19);
      break;
    case TerrainType::Water:
      color = GL::BLUE;
      break;
    case TerrainType::Stone:
      color = GL::GRAY;
      break;
  }

  if ((ns & Neighbors::Left) == Neighbors::None) {
    float top = accessibilityAt(ind.x - 1, ind.y + 1, ind.z);
    float bot = accessibilityAt(ind.x - 1, ind.y - 1, ind.z);
    float lft = accessibilityAt(ind.x - 1, ind.y, ind.z - 1);
    float rgt = accessibilityAt(ind.x - 1, ind.y, ind.z + 1);
    float toprgt = accessibilityAt(ind.x - 1, ind.y + 1, ind.z + 1);
    float toplft = accessibilityAt(ind.x - 1, ind.y + 1, ind.z - 1);
    float botrgt = accessibilityAt(ind.x - 1, ind.y - 1, ind.z + 1);
    float botlft = accessibilityAt(ind.x - 1, ind.y - 1, ind.z - 1);
    
    auto v1 = VERT(m, -1, -1, -1);
    auto v2 = VERT(m, -1, -1, 1);
    auto v3 = VERT(m, -1, 1, 1);
    auto v4 = VERT(m, -1, 1, -1);
    auto nrm = NORMAL(v1, v2, v3);
    v1.normal = v2.normal = v3.normal = v4.normal = nrm;
    v1.color = v2.color = v3.color = v4.color = color;
    v1.voxel_coordinate = v2.voxel_coordinate = v3.voxel_coordinate = v4.voxel_coordinate = ind;
    v1.ao_accessibility = bot * lft * botlft;
    v2.ao_accessibility = bot * rgt * botrgt;
    v3.ao_accessibility = top * rgt * toprgt;
    v4.ao_accessibility = top * lft * toplft;
    
    batch->add(v1);
    batch->add(v2);
    batch->add(v3);
    batch->add(v4);
    
    //For stats/debugging
    m_vertexCount += 4;
    
    batch->addIndex(v1.index);
    batch->addIndex(v2.index);
    batch->addIndex(v3.index);
    batch->addIndex(v1.index);
    batch->addIndex(v3.index);
    batch->addIndex(v4.index);
  }
  
  if ((ns & Neighbors::Back) == Neighbors::None) {
    float top = accessibilityAt(ind.x, ind.y + 1, ind.z - 1);
    float bot = accessibilityAt(ind.x, ind.y - 1, ind.z - 1);
    float lft = accessibilityAt(ind.x + 1, ind.y, ind.z - 1);
    float rgt = accessibilityAt(ind.x - 1, ind.y, ind.z - 1);
    float toplft = accessibilityAt(ind.x + 1, ind.y + 1, ind.z - 1);
    float toprgt = accessibilityAt(ind.x - 1, ind.y + 1, ind.z - 1);
    float botlft = accessibilityAt(ind.x + 1, ind.y - 1, ind.z - 1);
    float botrgt = accessibilityAt(ind.x - 1, ind.y - 1, ind.z - 1);
    
    auto v1 = VERT(m, 1, 1, -1);
    auto v2 = VERT(m, -1, -1, -1);
    auto v3 = VERT(m, -1, 1, -1);
    auto v4 = VERT(m, 1, -1, -1);
    v1.normal = v2.normal = v3.normal = v4.normal = NORMAL(v1, v2, v3);
    v1.color = v2.color = v3.color = v4.color = color;
    v1.voxel_coordinate = v2.voxel_coordinate = v3.voxel_coordinate = v4.voxel_coordinate = ind;
    v1.ao_accessibility = lft * top * toplft;
    v2.ao_accessibility = rgt * bot * botrgt;
    v3.ao_accessibility = rgt * top * toprgt;
    v4.ao_accessibility = lft * bot * botlft;
    
    batch->add(v1);
    batch->add(v2);
    batch->add(v3);
    batch->add(v4);
    
    //For stats/debugging
    m_vertexCount += 4;
    
    batch->addIndex(v1.index);
    batch->addIndex(v2.index);
    batch->addIndex(v3.index);
    batch->addIndex(v1.index);
    batch->addIndex(v4.index);
    batch->addIndex(v2.index);
  }
  
  if ((ns & Neighbors::Bottom) == Neighbors::None) {
    auto v1 = VERT(m, 1, -1, 1);
    auto v2 = VERT(m, -1, -1, -1);
    auto v3 = VERT(m, 1, -1, -1);
    auto v4 = VERT(m, -1, -1, 1);
    v1.normal = v2.normal = v3.normal = v4.normal = NORMAL(v1, v2, v3);
    v1.color = v2.color = v3.color = v4.color = color;
    v1.ao_accessibility = v2.ao_accessibility = v3.ao_accessibility = v4.ao_accessibility = 1.f;
    v1.voxel_coordinate = v2.voxel_coordinate = v3.voxel_coordinate = v4.voxel_coordinate = ind;
    
    batch->add(v1);
    batch->add(v2);
    batch->add(v3);
    batch->add(v4);
    
    //For stats/debugging
    m_vertexCount += 4;
    
    batch->addIndex(v1.index);
    batch->addIndex(v2.index);
    batch->addIndex(v3.index);
    batch->addIndex(v1.index);
    batch->addIndex(v4.index);
    batch->addIndex(v2.index);
  }
  
  if ((ns & Neighbors::Front) == Neighbors::None) {
    float top = accessibilityAt(ind.x, ind.y + 1, ind.z + 1);
    float bot = accessibilityAt(ind.x, ind.y - 1, ind.z + 1);
    float lft = accessibilityAt(ind.x - 1, ind.y, ind.z + 1);
    float rgt = accessibilityAt(ind.x + 1, ind.y, ind.z + 1);
    float botrgt = accessibilityAt(ind.x + 1, ind.y - 1, ind.z + 1);
    float botlft = accessibilityAt(ind.x - 1, ind.y - 1, ind.z + 1);
    float toprgt = accessibilityAt(ind.x + 1, ind.y + 1, ind.z + 1);
    float toplft = accessibilityAt(ind.x - 1, ind.y + 1, ind.z + 1);
    
    auto v1 = VERT(m, -1, 1, 1);
    auto v2 = VERT(m, -1, -1, 1);
    auto v3 = VERT(m, 1, -1, 1);
    auto v4 = VERT(m, 1, 1, 1);
    v1.normal = v2.normal = v3.normal = v4.normal = NORMAL(v1, v2, v3);
    v1.color = v2.color = v3.color = v4.color = color;
    v1.voxel_coordinate = v2.voxel_coordinate = v3.voxel_coordinate = v4.voxel_coordinate = ind;
    v1.ao_accessibility = lft * top * toplft;
    v2.ao_accessibility = lft * bot * botlft;
    v3.ao_accessibility = rgt * bot * botrgt;
    v4.ao_accessibility = rgt * top * toprgt;
    
    batch->add(v1);
    batch->add(v2);
    batch->add(v3);
    batch->add(v4);
    
    //For stats/debugging
    m_vertexCount += 4;
    
    batch->addIndex(v1.index);
    batch->addIndex(v2.index);
    batch->addIndex(v3.index);
    batch->addIndex(v4.index);
    batch->addIndex(v1.index);
    batch->addIndex(v3.index);
  }
  
  if ((ns & Neighbors::Right) == Neighbors::None) {
    float top = accessibilityAt(ind.x + 1, ind.y + 1, ind.z);
    float bot = accessibilityAt(ind.x + 1, ind.y - 1, ind.z);
    float lft = accessibilityAt(ind.x + 1, ind.y, ind.z + 1);
    float rgt = accessibilityAt(ind.x + 1, ind.y, ind.z - 1);
    float toplft = accessibilityAt(ind.x + 1, ind.y + 1, ind.z + 1);
    float toprgt = accessibilityAt(ind.x + 1, ind.y + 1, ind.z - 1);
    float botlft = accessibilityAt(ind.x + 1, ind.y - 1, ind.z + 1);
    float botrgt = accessibilityAt(ind.x + 1, ind.y - 1, ind.z - 1);
    
    auto v1 = VERT(m, 1, 1, 1);
    auto v2 = VERT(m, 1, -1, -1);
    auto v3 = VERT(m, 1, 1, -1);
    auto v4 = VERT(m, 1, -1, 1);
    v1.normal = v2.normal = v3.normal = v4.normal = NORMAL(v1, v2, v3);
    v1.color = v2.color = v3.color = v4.color = color;
    v1.voxel_coordinate = v2.voxel_coordinate = v3.voxel_coordinate = v4.voxel_coordinate = ind;
    v1.ao_accessibility = top * lft * toplft;
    v2.ao_accessibility = bot * rgt * botrgt;
    v3.ao_accessibility = top * rgt * toprgt;
    v4.ao_accessibility = bot * lft * botlft;
    
    batch->add(v1);
    batch->add(v2);
    batch->add(v3);
    batch->add(v4);
    
    //For stats/debugging
    m_vertexCount += 4;
    
    batch->addIndex(v1.index);
    batch->addIndex(v2.index);
    batch->addIndex(v3.index);
    batch->addIndex(v2.index);
    batch->addIndex(v1.index);
    batch->addIndex(v4.index);
  }
  
  if ((ns & Neighbors::Top) == Neighbors::None) {
    float lft = accessibilityAt(ind.x - 1, ind.y + 1, ind.z);
    float rgt = accessibilityAt(ind.x + 1, ind.y + 1, ind.z);
    float fnt = accessibilityAt(ind.x, ind.y + 1, ind.z + 1);
    float bck = accessibilityAt(ind.x, ind.y + 1, ind.z - 1);
    float fntlft = accessibilityAt(ind.x - 1, ind.y + 1, ind.z + 1);
    float fntrgt = accessibilityAt(ind.x + 1, ind.y + 1, ind.z + 1);
    float bcklft = accessibilityAt(ind.x - 1, ind.y + 1, ind.z - 1);
    float bckrgt = accessibilityAt(ind.x + 1, ind.y + 1, ind.z - 1);
    
    auto v1 = VERT(m, 1, 1, 1);
    auto v2 = VERT(m, 1, 1, -1);
    auto v3 = VERT(m, -1, 1, -1);
    auto v4 = VERT(m, -1, 1, 1);
    v1.normal = v2.normal = v3.normal = v4.normal = NORMAL(v1, v2, v3);
    v1.color = v2.color = v3.color = v4.color = color;
    v1.voxel_coordinate = v2.voxel_coordinate = v3.voxel_coordinate = v4.voxel_coordinate = ind;
    v1.ao_accessibility = rgt * fnt * fntrgt;
    v2.ao_accessibility = rgt * bck * bckrgt;
    v3.ao_accessibility = lft * bck * bcklft;
    v4.ao_accessibility = lft * fnt * fntlft;
    
    batch->add(v1);
    batch->add(v2);
    batch->add(v3);
    batch->add(v4);
    
    //For stats/debugging
    m_vertexCount += 4;
    
    batch->addIndex(v1.index);
    batch->addIndex(v2.index);
    batch->addIndex(v3.index);
    batch->addIndex(v1.index);
    batch->addIndex(v3.index);
    batch->addIndex(v4.index);
  }
}

void Chunk::draw(Env &env,
                 const glm::vec4 &playerPos,
                 const glm::vec3 &playerLookVec,
                 bool isHeadlightOn,
                 bool exploding,
                 GLclampf explosionTime)
{
  if (!m_generated)
    return;
  
  //Update environment stats
  debug_stats &stats = env.getStats();
  stats.vertices += m_vertexCount;
  stats.voxels += CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;
  stats.active_voxels += m_activeVoxels;
  stats.drawn_voxels += m_drawnVoxels;
  
  MatrixStack &mv = env.getMV();
  ShaderSet &shaders = env.getShaders();
  GLfloat offset = CHUNK_SIZE * VOXEL_SIZE;
  mv.pushMatrix();
  {
    mv.translate(m_chunkIndex.x * offset, 0.f, m_chunkIndex.y * offset);
    glm::vec4 groundColor = m_containsPlayer ? glm::vec4(0.0f, 0.0f, 0.3f, 1.f) : GL::color(51, 102, 51);
    shaders.prepareHemisphereAO(env,
                                glm::vec4(.8f, .8f, .8f, 1.f),
                                groundColor,
                                exploding,
                                explosionTime);
    m_voxelBatch->draw(GL_TRIANGLES);
  }
  mv.popMatrix();
}

bool Chunk::isVoxelActiveAt(voxel_coord_type x, voxel_coord_type y, voxel_coord_type z)
{
  voxel_key_type key = hashCoords(x, y, z);
  std::map<voxel_key_type, bool>::iterator it = m_voxelMap.find(key);
  return it != m_voxelMap.end();
}

GLclampf Chunk::accessibilityAt(voxel_coord_type x, voxel_coord_type y, voxel_coord_type z)
{
  if (x < 0 || x > CHUNK_SIZE - 1)
    return 1.f;
  if (y < 0 || y > CHUNK_SIZE - 1)
    return 1.f;
  if (z < 0 || z > CHUNK_SIZE - 1)
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

//Safe to invoke on a worker thread
void Chunk::generateData()
{
  /* Generation strategy:
   We use the chunk's indices as bounds.
   So for any given chunk, we use noise values in the interval
   [chunk.x, chunk.x + 1], [0,1], [chunk.y, chunk.y + 1],
   where the 'y' is actually a z-index.
   We map the range of the noise function (which is in the interval
   [-1,1]) to a value in the range [0,31], where each voxel below this
   value is active and each one above is not active.
   Voxels are indexed starting at the bottom left corner of the chunk.
   In most cases, we only need a fraction of the total capacity of the chunk -- 
   all of the empty voxels can be discarded, as well as the totally occluded 
   ones.  How do we initially store them, however (before determining which ones
   can be discarded)?
  */
  m_voxelBatch = new VertexBatch();
  vertex_spec_t &spec = m_voxelBatch->getVertexSpec();
  spec.indexed = true;
  spec.use_ao = true;
  spec.use_color = true;
  spec.use_voxel_coordinates = true;
  m_voxelBatch->begin();
  
  //Translate to the bottom left
  MatrixStack stack;
  GLfloat offset = CHUNK_SIZE * .5f - VOXEL_SIZE * .5f;
  stack.translate(-offset, -offset, offset);
  //Should now be at the origin of voxel (0,0,0)
  
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
  
  //noise::module::Turbulence finalTerrain;
  //finalTerrain.SetSourceModule(0, terrainSelector);
  //finalTerrain.SetFrequency(4.0);
  //finalTerrain.SetPower(0.125);
  
  utils::NoiseMap heightMap;
  utils::NoiseMapBuilderPlane heightMapBuilder;
  heightMapBuilder.SetSourceModule(terrainSelector);
  heightMapBuilder.SetDestNoiseMap(heightMap);
  heightMapBuilder.SetDestSize(CHUNK_SIZE, CHUNK_SIZE);
  heightMapBuilder.SetBounds(m_chunkIndex.x,
                             m_chunkIndex.x + 1,
                             m_chunkIndex.y,
                             m_chunkIndex.y + 1);
  heightMapBuilder.Build();
  
  //First pass - enable/disable voxels using the height map
  for (int i = 0; i < CHUNK_SIZE; i++) {
    for (int j = 0; j < CHUNK_SIZE; j++) {
      //Calculate how many voxels are active in this column
      GLfloat noise = heightMap.GetValue(i, j);
      int ht = (CHUNK_SIZE * .5) * noise;
      ht += CHUNK_SIZE * .5;
      if (!ht) ht = 1;
      
      //Set voxels active/inactive
      for (int v = 0; v < CHUNK_SIZE; v++) {
        if (v < ht) {
          //Enable
          m_activeVoxels++;
          m_voxelMap[hashCoords(i, v, j)] = true;
        } else {
          break;
        }
      }
    }
  }
  
  //Second pass -- figure out neighbors so
  //we can avoid drawing unnecessary vertices,
  //and assign voxel types
  Neighbors ns = Neighbors::Bottom;
  TerrainType type;
  GLfloat noise;
  unsigned ht;
  for (voxel_coord_type i = 0; i < CHUNK_SIZE; i++) {
    stack.translateX(VOXEL_SIZE);
    stack.pushMatrix();
    {
      for (voxel_coord_type j = 0; j < CHUNK_SIZE; j++) {
        stack.translateZ(VOXEL_SIZE);
        
        //Calculate neighbors
        stack.pushMatrix();
        {
          for (voxel_coord_type v = 0; v < CHUNK_SIZE; v++) {
            stack.translateY(VOXEL_SIZE);
            noise = heightMap.GetValue(i, j);
            ht = (CHUNK_SIZE * .5) * noise;
            ht += CHUNK_SIZE * .5;
            if (!ht) ht = 1;
            if (isVoxelActiveAt(i, v, j)) {
              ns = Neighbors::Bottom;
              if (v < CHUNK_SIZE - 1 && isVoxelActiveAt(i, v + 1, j)) {
                ns = ns | Neighbors::Top;
                type = TerrainType::Dirt;
              } else if (v > 0) {
                //Grass, unless we are at a low enough altitude
                type = TerrainType::Grass;
              } else {
                type = TerrainType::Water;
              }
              
              if (i > 0 && isVoxelActiveAt(i - 1, v, j))
                ns = ns | Neighbors::Left;
              if (i < CHUNK_SIZE - 1 && isVoxelActiveAt(i + 1, v, j))
                ns = ns | Neighbors::Right;
              if (j > 0 && isVoxelActiveAt(i, v, j - 1))
                ns = ns | Neighbors::Back;
              if (j < CHUNK_SIZE - 1 && isVoxelActiveAt(i, v, j + 1))
                ns = ns | Neighbors::Front;

              stack.pushMatrix();
              {
                stack.scale(VOXEL_SIZE * .5f, VOXEL_SIZE * .5f, VOXEL_SIZE * .5f);
                addVoxel(glm::vec3(i, v, j), ns, type, m_voxelBatch, stack);
              }
              stack.popMatrix();
            } else {
              //We can't currently have any active voxels above an inactive
              //one, so break
              break;
            }
          }
        }
        stack.popMatrix();
      }
    }
    stack.popMatrix();
  }
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
  m_voxelBatch->end();
  m_voxelMap.clear();
  m_generated = true;
  return true;
}








