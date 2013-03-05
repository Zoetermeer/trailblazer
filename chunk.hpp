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

enum class Neighbors {
  None = 0,
  Left = 1,
  Right = 2,
  Top = 4,
  Bottom = 8,
  Front = 16,
  Back = 32
};

inline Neighbors operator&(Neighbors a, Neighbors b)
{
  typedef std::underlying_type<Neighbors>::type enum_type;
  return static_cast<Neighbors>(static_cast<enum_type>(a) & static_cast<enum_type>(b));
}

inline Neighbors operator|(Neighbors a, Neighbors b)
{
  typedef std::underlying_type<Neighbors>::type enum_type;
  return static_cast<Neighbors>(static_cast<enum_type>(a) | static_cast<enum_type>(b));
}

class Voxel {
private:
  glm::ivec3 m_index;
  bool m_isActive;
  Neighbors m_neighbors;
  
public:
  Voxel()
  : m_isActive(false), m_neighbors(Neighbors::None)
  {
    
  }
  
  bool getIsActive() const { return m_isActive; }
  Neighbors getNeighbors() const { return m_neighbors; }
  void setNeighbors(Neighbors n) { m_neighbors = n; }
  
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