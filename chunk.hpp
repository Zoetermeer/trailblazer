#ifndef CHUNK_H
#define CHUNK_H

#include <glm/glm.hpp>
#include "scene-object.hpp"
#include "env.hpp"
#include "vertex-batch.hpp"

//Number of voxels per chunk face
#define CHUNK_SIZE 32

//Size (in GL units) of a voxel face
#define VOXEL_SIZE 5.f

class Voxel {
private:
  glm::ivec3 m_index;
  bool m_isActive;
  
public:
  Voxel()
  : m_isActive(false)
  {
    
  }
  
  bool getIsActive() const { return m_isActive; }
  
  void setIndex(int x, int y, int z)
  {
    m_index.x = x;
    m_index.y = y;
    m_index.z = z;
  }
  
  void setIsActive(bool v) { m_isActive = v; }
};

class Chunk : public SceneObject {
private:
  bool m_generated;
  Voxel m_voxels[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
  VertexBatch *m_vbo;
  glm::ivec2 m_chunkIndex;
  
public:
  Chunk()
  : Chunk(0, 0)
  {
    m_vbo = NULL;
  }
  
  Chunk(int xIndex, int zIndex)
  : SceneObject(), m_generated(false), m_chunkIndex(xIndex, zIndex)
  {
    
  }
  
  ~Chunk()
  {
    if (m_vbo)
      delete m_vbo;
  }
  
public:
  glm::ivec2 getIndex() const { return m_chunkIndex; }
  void setIndex(int x, int z)
  {
    m_chunkIndex.x = x;
    m_chunkIndex.y = z;
  }
  
public:
  void generate();
  virtual void draw(Env &env);
};

#endif