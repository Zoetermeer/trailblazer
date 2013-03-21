#include "chunk.hpp"
#include "matrix-stack.hpp"
#include "shader.hpp"
#include "GL.hpp"
#include "sky.hpp"
#include <noise.h>
#include "noiseutils.h"
#include <chrono>

//This function should be thread-safe
bool Chunk::isVoxelActiveAt(voxel_coord_type x, voxel_coord_type y, voxel_coord_type z)
{
  voxel_key_type key = hashCoords(x, y, z);
  std::map<voxel_key_type, bool>::iterator it = m_voxelMap.find(key);
  return it != m_voxelMap.end();
}

GLclampf Chunk::accessibilityAt(voxel_coord_type x, voxel_coord_type y, voxel_coord_type z)
{
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
  m_vertexBuffer->end();
  m_voxelMap.clear();
  m_generated = true;
  return true;
}








