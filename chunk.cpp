#include "chunk.hpp"
#include "matrix-stack.hpp"
#include "shader.hpp"
#include "GL.hpp"
#include "sky.hpp"
#include <noise.h>
#include "noiseutils.h"

#define VERT(m,x,y,z) (new_vertex((m * glm::vec4(x,y,z,1.f))))
#define NORMAL(a,b,c) (glm::normalize(glm::cross(glm::vec3(b.position - a.position), glm::vec3(c.position - a.position))))
void Chunk::addVoxel(Voxel &voxel,
                     VertexBatch *batch,
                     MatrixStack &stack)
{
  glm::mat4 &m = stack.current();
  glm::ivec3 ind = voxel.getIndex();
  int x = ind.x, y = ind.y, z = ind.z;
  Neighbors ns = voxel.getNeighbors();
  float accessibility = 1.f;
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
    v1.ao_accessibility = bot * lft * botlft;
    v2.ao_accessibility = bot * rgt * botrgt;
    v3.ao_accessibility = top * rgt * toprgt;
    v4.ao_accessibility = top * lft * toplft;
    
    batch->add(v1);
    batch->add(v2);
    batch->add(v3);
    batch->add(v4);
    
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
    v1.ao_accessibility = lft * top * toplft;
    v2.ao_accessibility = rgt * bot * botrgt;
    v3.ao_accessibility = rgt * top * toprgt;
    v4.ao_accessibility = lft * bot * botlft;
    
    batch->add(v1);
    batch->add(v2);
    batch->add(v3);
    batch->add(v4);
    
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
    v1.ao_accessibility = v2.ao_accessibility = v3.ao_accessibility = v4.ao_accessibility = 1.f;
    
    batch->add(v1);
    batch->add(v2);
    batch->add(v3);
    batch->add(v4);
    
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
    v1.ao_accessibility = lft * top * toplft;
    v2.ao_accessibility = lft * bot * botlft;
    v3.ao_accessibility = rgt * bot * botrgt;
    v4.ao_accessibility = rgt * top * toprgt;
    
    batch->add(v1);
    batch->add(v2);
    batch->add(v3);
    batch->add(v4);
    
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
    v1.ao_accessibility = top * lft * toplft;
    v2.ao_accessibility = bot * rgt * botrgt;
    v3.ao_accessibility = top * rgt * toprgt;
    v4.ao_accessibility = bot * lft * botlft;
    
    batch->add(v1);
    batch->add(v2);
    batch->add(v3);
    batch->add(v4);
    
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
    v1.ao_accessibility = rgt * fnt * fntrgt;
    v2.ao_accessibility = rgt * bck * bckrgt;
    v3.ao_accessibility = lft * bck * bcklft;
    v4.ao_accessibility = lft * fnt * fntlft;
    
    batch->add(v1);
    batch->add(v2);
    batch->add(v3);
    batch->add(v4);
    
    batch->addIndex(v1.index);
    batch->addIndex(v2.index);
    batch->addIndex(v3.index);
    batch->addIndex(v1.index);
    batch->addIndex(v3.index);
    batch->addIndex(v4.index);
  }
}

void Chunk::generate()
{
  if (m_generated)
    return;
  
  /* Generation strategy:
    We use the chunk's indices as bounds.
    So for any given chunk, we use noise values in the interval 
    [chunk.x, chunk.x + 1], [0,1], [chunk.y, chunk.y + 1], 
    where the 'y' is actually a z-index.
    We map the range of the noise function (which is in the interval 
    [-1,1] to a value in the range [0,31], where each voxel below this
    value is active and each one above is not active.
    Voxels are indexed starting at the bottom left corner of the chunk.
   */
  m_vbo = new VertexBatch();
  m_vbo->getVertexSpec().indexed = true;
  m_vbo->getVertexSpec().use_ao = true;
  m_vbo->begin();
  
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
  heightMapBuilder.SetDestSize(32, 32);
  heightMapBuilder.SetBounds(m_chunkIndex.x,
                             m_chunkIndex.x + 1,
                             m_chunkIndex.y,
                             m_chunkIndex.y + 1);
  heightMapBuilder.Build();
  for (int i = 0; i < CHUNK_SIZE; i++) {
    for (int j = 0; j < CHUNK_SIZE; j++) {
      //Calculate how many voxels are active in this column
      GLfloat noise = heightMap.GetValue(i, j);
      int ht = (CHUNK_SIZE * .5) * noise;
      ht += CHUNK_SIZE * .5;
      if (!ht) ht = 1;
      
      //Set voxels active/inactive
      for (int v = 0; v < CHUNK_SIZE; v++) {
        Voxel &voxel = m_voxels[i][v][j];
        voxel.setIndex(i, v, j);
        if (v < ht)
          voxel.setIsActive(true);
        else
          voxel.setIsActive(false);
      }
    }
  }
  
  //Second pass -- figure out neighbors so
  //we can avoid drawing unnecessary vertices
  for (int i = 0; i < CHUNK_SIZE; i++) {
    stack.translateX(VOXEL_SIZE);
    stack.pushMatrix();
    {
      for (int j = 0; j < CHUNK_SIZE; j++) {
        stack.translateZ(VOXEL_SIZE);
        
        //Calculate neighbors
        stack.pushMatrix();
        {
          for (int v = 0; v < CHUNK_SIZE; v++) {
            stack.translateY(VOXEL_SIZE);
            Voxel &voxel = m_voxels[i][v][j];
            voxel.setNeighbors(Neighbors::Bottom);
            if (voxel.getIsActive()) {
              Neighbors ns = voxel.getNeighbors();
              ns = ns | Neighbors::Bottom;
              if (i > 0 && m_voxels[i - 1][v][j].getIsActive())
                ns = ns | Neighbors::Left;
              if (i < CHUNK_SIZE - 1 && m_voxels[i + 1][v][j].getIsActive())
                ns = ns | Neighbors::Right;
              if (v < CHUNK_SIZE - 1 && m_voxels[i][v + 1][j].getIsActive())
                ns = ns | Neighbors::Top;
              if (j > 0 && m_voxels[i][v][j - 1].getIsActive())
                ns = ns | Neighbors::Back;
              if (j < CHUNK_SIZE - 1 && m_voxels[i][v][j + 1].getIsActive())
                ns = ns | Neighbors::Front;
              
              voxel.setNeighbors(ns);
              stack.pushMatrix();
              {
                stack.scale(VOXEL_SIZE * .5f, VOXEL_SIZE * .5f, VOXEL_SIZE * .5f);
                addVoxel(voxel, m_vbo, stack);
              }
              stack.popMatrix();
            }
          }
        }
        stack.popMatrix();
      }
    }
    stack.popMatrix();
  }
  
  m_vbo->end();
  m_generated = true;
}

void Chunk::draw(Env &env)
{
  if (!m_generated)
    return;
  
  MatrixStack &mv = env.getMV();
  ShaderSet &shaders = env.getShaders();
  GLfloat offset = CHUNK_SIZE * VOXEL_SIZE;
  mv.pushMatrix();
  {
    mv.translate(m_chunkIndex.x * offset, 0.f, m_chunkIndex.y * offset);
    glm::vec4 groundColor = m_containsPlayer ? glm::vec4(0.0f, 0.0f, 0.3f, 1.f) : GL::color(51, 102, 51);
    
    glm::vec3 sunPos = glm::vec3(Sky::getSunPosition());
    shaders.prepareHemisphereAO(env, sunPos, glm::vec4(.8f, .8f, .8f, 1.f), groundColor);
    m_vbo->draw(GL_TRIANGLES);
  }
  mv.popMatrix();
}

GLclampf Chunk::accessibilityAt(int x, int y, int z)
{
  if (x < 0 || x > CHUNK_SIZE - 1)
    return 1.f;
  if (y < 0 || y > CHUNK_SIZE - 1)
    return 1.f;
  if (z < 0 || z > CHUNK_SIZE - 1)
    return 1.f;
  
  if (m_voxels[x][y][z].getIsActive())
    return .9f;
  
  return 1.f;
}

void ChunkBuffer::init()
{
  //Load a 5x5 grid, centered on the world-space origin
  int halfDown = floor((GLfloat)VISIBLE_CHUNKS * .5);
  for (int i = -halfDown; i <= halfDown; i++) {
    for (int j = -halfDown; j <= halfDown; j++) {
      Chunk *ch = new Chunk(i, j);
      m_loadQueue.push_back(ch);
    }
  }
}

void ChunkBuffer::removeChunksAtX(int xIndex)
{
  std::vector<Chunk*>::iterator iter = m_visibleQueue.begin();
  while (iter != m_visibleQueue.end()) {
    Chunk *ch = *iter;
    if (ch->getIndex().x == xIndex) {
      iter = m_visibleQueue.erase(iter);
      delete ch;
      continue;
    }
    
    ++iter;
  }
}

void ChunkBuffer::removeChunksAtZ(int zIndex)
{
  std::vector<Chunk*>::iterator iter = m_visibleQueue.begin();
  while (iter != m_visibleQueue.end()) {
    Chunk *ch = *iter;
    if (ch->getIndex().y == zIndex) {
      iter = m_visibleQueue.erase(iter);
      delete ch;
      continue;
    }
    
    ++iter;
  }
}

void ChunkBuffer::onPlayerMove(const glm::vec4 &old_pos, const glm::vec4 &new_pos)
{
  glm::ivec3 chunkCoords = Chunk::worldToChunkSpace(glm::vec3(new_pos));
  if (chunkCoords.x != m_curPlayerChunkCoords.x || chunkCoords.z != m_curPlayerChunkCoords.z) {
    //Set the new chunk as the player's 'current' one
    Chunk *newCh = this->chunkAt(chunkCoords.x, chunkCoords.z);
    if (m_curPlayerChunk)
      m_curPlayerChunk->setContainsPlayer(false);
    
    if (newCh) {
      m_curPlayerChunk = newCh;
      newCh->setContainsPlayer(true);
    }
    
    glm::ivec3 delta = chunkCoords - m_curPlayerChunkCoords;
    int halfUp = ceil((GLfloat)VISIBLE_CHUNKS * .5);
    int halfDown = floor((GLfloat)VISIBLE_CHUNKS * .5);
    if (delta.x != 0) {
      //Remove in x dimension
      int x = chunkCoords.x - (delta.x * halfUp);
      removeChunksAtX(x);
      for (int i = chunkCoords.z - halfDown; i <= chunkCoords.z + halfDown; i++) {
        m_loadQueue.push_back(new Chunk(chunkCoords.x + (delta.x * halfDown), i));
      }
    }
    
    if (delta.z != 0) {
      //Remove in z dimension
      int z = chunkCoords.z - (delta.z * halfUp);
      removeChunksAtZ(z);
      for (int i = chunkCoords.x - halfDown; i <= chunkCoords.x + halfDown; i++) {
        m_loadQueue.push_back(new Chunk(i, chunkCoords.z + (delta.z * halfDown)));
      }
    }
    
    m_curPlayerChunkCoords = chunkCoords;
  }
}

void ChunkBuffer::advance(int delta)
{
  //If the load list is non-empty, load 1 chunk
  if (m_loadQueue.size() > 0) {
    Chunk *ch = m_loadQueue.back();
    m_loadQueue.pop_back();
    ch->generate();
    m_visibleQueue.push_back(ch);
  }
}

bool ChunkBuffer::isDone()
{
  return false;
}

void ChunkBuffer::draw(Env &env)
{
  for (Chunk *ch : m_visibleQueue) {
    ch->draw(env);
  }
}

Chunk *ChunkBuffer::chunkAt(int x, int z)
{
  for (Chunk *ch : m_visibleQueue) {
    glm::ivec2 ind = ch->getIndex();
    if (ind.x == x && ind.y == z)
      return ch;
  }
  
  //TODO: Look in the load queue -- if not there,
  //then we need to create it
  return NULL;
}









