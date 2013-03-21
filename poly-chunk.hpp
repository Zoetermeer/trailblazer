#ifndef POLY_CHUNK_H
#define POLY_CHUNK_H

#include "chunk.hpp"

class PolyChunk : public Chunk {
public:
  PolyChunk(int xIndex, int yIndex, int zIndex)
  : Chunk(xIndex, yIndex, zIndex)
  {
    
  }
  
  ~PolyChunk()
  {
    
  }
  
protected:
  virtual GLclampf accessibilityAt(voxel_coord_type x, voxel_coord_type y, voxel_coord_type z);
  void addCube(const glm::vec3 &ind,
               const Neighbors &ns,
               const TerrainType &type,
               VertexBatch *batch,
               MatrixStack &stack);
  
  virtual void generateData();
};

#endif