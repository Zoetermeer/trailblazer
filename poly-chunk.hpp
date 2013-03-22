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
  
private:
  glm::vec3 normalForQuad(voxel_coord_type x, voxel_coord_type z);
  glm::vec3 vertexNormal(byte localIndex, glm::vec3 quadNorm, voxel_coord_type vx, voxel_coord_type vz);
  
protected:
  virtual GLclampf accessibilityAt(voxel_coord_type x, voxel_coord_type y, voxel_coord_type z);  
  virtual void generateData();
};

#endif