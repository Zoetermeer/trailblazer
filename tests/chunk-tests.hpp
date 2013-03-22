#ifndef CHUNK_TESTS_H
#define CHUNK_TESTS_H

#include <gtest/gtest.h>
#include "printer.hpp"
#include "../chunk.hpp"

TEST(ChunkTests, WorldToChunkCoordsTest1) {
  glm::vec3 playerPos(1.0, 1.0, -1.0);
  EXPECT_EQ(glm::ivec3(0, 0, 0), Chunk::worldToChunkSpace(playerPos));
}

TEST(ChunkTests, WorldToChunkCoordsTest2) {
  glm::vec3 playerPos = glm::vec3(0.1, 0.0, -(VOXEL_SIZE * VOXELS_PER_CHUNK + (VOXEL_SIZE * .5)));
  EXPECT_EQ(glm::ivec3(0, 0, 1), Chunk::worldToChunkSpace(playerPos));
}

//Use a mock chunk class to test the chunk buffer
class DummyChunk : public Chunk {
public:
  DummyChunk(int xIndex, int yIndex, int zIndex)
  : Chunk(xIndex, yIndex, zIndex)
  {
    
  }
  
  ~DummyChunk()
  {
    
  }
  
protected:
  virtual void generateData()
  {
    
  }
};

TEST(ChunkTests, BufferMaintainsProperSize) {
  ChunkBuffer<DummyChunk> buf;
  buf.init();
  
  //Simulate a player move
  Events::playerMoveEvent(glm::vec4(0, 0, 0, 1), glm::vec4(0, 0, -1, 1));
  EXPECT_EQ(VISIBLE_CHUNKS * VISIBLE_CHUNKS, buf.getLoadQueueSize() + buf.getRenderQueueSize());
}
#endif