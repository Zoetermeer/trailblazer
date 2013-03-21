#include "poly-chunk.hpp"

GLclampf PolyChunk::accessibilityAt(voxel_coord_type x, voxel_coord_type y, voxel_coord_type z)
{
  return 1.f;
}

void PolyChunk::addCube(const glm::vec3 &ind,
                        const Neighbors &ns,
                        const TerrainType &type,
                        VertexBatch *batch,
                        MatrixStack &stack)
{
  
}

void PolyChunk::generateData()
{
  auto vb = new VertexBatch();
  setVertexBuffer(vb);
  vb->begin();
  
  
}

void PolyChunk::draw(Env &env,
                     const glm::vec4 &playerPos,
                     const glm::vec3 &playerLookVec,
                     bool isHeadlightOn,
                     bool exploding,
                     GLclampf explosionTime)
{
  
}