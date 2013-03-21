#ifndef CHUNK_H
#define CHUNK_H

#include <glm/glm.hpp>
#include "scene-object.hpp"
#include "env.hpp"
#include "vertex-batch.hpp"
#include "events.hpp"
#include "process.hpp"
#include "noiseutils.h"
#include <vector>
#include <future>
#include <map>

//Number of parallel tasks we use when generating
//chunk data
#define PARALLEL_GENERATORS 4

#define VOXELS_PER_CHUNK 250

//Size (in GL units) of a voxel face
#define VOXEL_SIZE 5.f

//The number of chunks visible in a given dimension.
//So with 5, we see the one we're currently in, plus 2 on
//either side, 2 in front, 2 behind.
#define VISIBLE_CHUNKS 3

//Use a z-order curve to hash active voxels,
//so we can build up a sparse matrix (using a dictionary
//of keys).  The key type
//must be big enough (in bits) to fit the concatenation
//of the x,y,z coordinates.
typedef unsigned char voxel_coord_type;
typedef unsigned voxel_key_type;

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
  RightBack = Right | Back,
  All = Left | Right | Top | Bottom | Front | Back
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

//Abstract class
class Chunk {
private:
  utils::NoiseMap m_heightMap;
  bool m_generated;
  std::map<voxel_key_type, bool> m_voxelMap;
  VertexBatch *m_vertexBuffer;
  glm::ivec3 m_chunkIndex;
  bool m_containsPlayer;
  
  bool m_generatingAsync;
  std::future<void> m_generatingFuture;
  
  //Stats for debugging
  unsigned m_vertexCount;
  unsigned m_activeVoxels;
  unsigned m_drawnVoxels;
  
protected:
  Chunk(int xIndex, int yIndex, int zIndex)
  : m_generated(false),
    m_chunkIndex(xIndex, yIndex, zIndex),
    m_containsPlayer(false),
    m_vertexBuffer(NULL),
    m_generatingAsync(false),
    m_vertexCount(0),
    m_activeVoxels(0),
    m_drawnVoxels(0)
  {
    
  }
  
  ~Chunk()
  {
    if (m_vertexBuffer)
      delete m_vertexBuffer;
  }
  
protected:
  inline VertexBatch *getVertexBuffer() const { return m_vertexBuffer; }
  inline void setVertexBuffer(VertexBatch *buf) { m_vertexBuffer = buf; }
  inline std::map<voxel_key_type, bool> &getVoxelMap() { return m_voxelMap; }
  inline void incrVertexCount(unsigned incr) { m_vertexCount += incr; }
  inline void incrActiveVoxels(unsigned incr) { m_activeVoxels += incr; }
  inline void incrDrawnVoxels(unsigned incr) { m_drawnVoxels += incr; }
  inline unsigned getVertexCount() const { return m_vertexCount; }
  inline unsigned getActiveVoxels() const { return m_activeVoxels; }
  inline unsigned getDrawnVoxels() const { return m_drawnVoxels; }
  
public:
  glm::ivec3 getIndex() const { return m_chunkIndex; }
  void setIndex(int x, int y, int z)
  {
    m_chunkIndex.x = x;
    m_chunkIndex.y = y;
    m_chunkIndex.y = z;
  }
  
  virtual bool isVoxelActiveAt(voxel_coord_type x, voxel_coord_type y, voxel_coord_type z);
  
  bool isGenerated() const { return m_generated; }
  bool containsPlayer() const { return m_containsPlayer; }
  void setContainsPlayer(bool v) { m_containsPlayer = v; }
  
  static glm::ivec3 worldToChunkSpace(glm::vec3 wc)
  {
    const GLfloat FACTOR = VOXELS_PER_CHUNK * VOXELS_PER_CHUNK;
    glm::vec3 v(wc.x / FACTOR, wc.y / FACTOR, wc.z / FACTOR);
    
    return glm::ivec3(ceil(wc.x / FACTOR) - 1, ceil(wc.y / FACTOR) - 1, ceil(wc.z / FACTOR) - 1);
  }
  
protected:
  virtual GLclampf accessibilityAt(voxel_coord_type x, voxel_coord_type y, voxel_coord_type z);  
  virtual void generateData();
  voxel_key_type hashCoords(voxel_coord_type x, voxel_coord_type y, voxel_coord_type z);
  GLfloat heightAt(int x, int z);
  
  
public:
  static void doGenerate(Chunk *chunk);
  bool generateDataAsync();
  virtual void draw(Env &env,
                    const glm::vec4 &playerPos,
                    const glm::vec3 &playerLookVec,
                    bool isHeadlightOn,
                    bool exploding,
                    GLclampf explosionTime);
};

#endif