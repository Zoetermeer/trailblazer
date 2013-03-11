#ifndef CHUNK_H
#define CHUNK_H

#include <glm/glm.hpp>
#include "scene-object.hpp"
#include "env.hpp"
#include "vertex-batch.hpp"
#include "events.hpp"
#include "process.hpp"
#include <cmath>
#include <vector>

//Number of voxels per chunk face
#define CHUNK_SIZE 32

//Size (in GL units) of a voxel face
#define VOXEL_SIZE 5.f

//The number of chunks visible in a given dimension.
//So with 5, we see the one we're currently in, plus 2 on
//either side.
#define VISIBLE_CHUNKS 11

enum class TerrainType {
  Grass,
  Dirt,
  Stone,
  Water
};

enum class Neighbors {
  None = 0,
  Left = 1,
  Right = 2,
  Top = 4,
  Bottom = 8,
  Front = 16,
  Back = 32,
  LeftFront = Left | Front,
  RightFront = Right | Front,
  LeftBack = Left | Back,
  RightBack = Right | Back
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
  TerrainType m_type;
  
public:
  Voxel()
  : m_isActive(false), m_neighbors(Neighbors::None)
  {
    
  }
  
  bool getIsActive() const { return m_isActive; }
  Neighbors getNeighbors() const { return m_neighbors; }
  void setNeighbors(Neighbors n) { m_neighbors = n; }
  TerrainType getTerrainType() const { return m_type; }
  void setTerrainType(TerrainType t) { m_type = t; }
  
  glm::ivec3 getIndex() const { return m_index; }
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
  VertexBatch *m_voxelBatch;
  VertexBatch *m_treeBatch;
  glm::ivec2 m_chunkIndex;
  bool m_containsPlayer;
  
public:
  Chunk(int xIndex, int zIndex)
  : SceneObject(),
    m_generated(false),
    m_chunkIndex(xIndex, zIndex),
    m_containsPlayer(false),
    m_voxelBatch(NULL),
    m_treeBatch(NULL)
  {
    
  }
  
  ~Chunk()
  {
    if (m_voxelBatch)
      delete m_voxelBatch;
    
    if (m_treeBatch)
      delete m_treeBatch;
  }
  
public:
  glm::ivec2 getIndex() const { return m_chunkIndex; }
  void setIndex(int x, int z)
  {
    m_chunkIndex.x = x;
    m_chunkIndex.y = z;
  }
  
  bool getIsGenerated() const { return m_generated; }
  bool getContainsPlayer() const { return m_containsPlayer; }
  void setContainsPlayer(bool v) { m_containsPlayer = v; }
  
  static glm::ivec3 worldToChunkSpace(glm::vec3 wc)
  {
    const GLfloat FACTOR = CHUNK_SIZE * VOXEL_SIZE;
    glm::vec3 v(wc.x / FACTOR, wc.y / FACTOR, wc.z / FACTOR);

    return glm::ivec3(ceil(wc.x / FACTOR) - 1, ceil(wc.y / FACTOR) - 1, ceil(wc.z / FACTOR) - 1);
  }
  
private:
  GLclampf accessibilityAt(int x, int y, int z);
  void addVoxel(Voxel &voxel, VertexBatch *batch, MatrixStack &stack);
  
public:
  void generate();
  virtual void draw(Env &env);
};

class ChunkBuffer : public IPlayerMoveListener, public Process, public SceneObject {
private:
  glm::ivec3 m_curPlayerChunkCoords;
  std::vector<Chunk*> m_loadQueue;
  std::vector<Chunk*> m_visibleQueue;
  Chunk *m_curPlayerChunk;
  
public:
  ChunkBuffer()
  : m_curPlayerChunk(NULL)
  {
    Events::addListener((IPlayerMoveListener*)this);
    ProcessList::add(this);
  }
  
  ~ChunkBuffer()
  {
    ProcessList::remove(this);
    Events::removeListener(EventPlayerMove, this);
  }
  
private:
  void removeChunksAtX(int xIndex);
  void removeChunksAtZ(int zIndex);
  
public:
  void init();
  virtual void onPlayerMove(const glm::vec4 &old_pos, const glm::vec4 &new_pos);
  virtual void draw(Env &env);
  virtual void advance(int delta);
  virtual bool isDone();
  
  Chunk *chunkAt(int x, int z);
};

#endif