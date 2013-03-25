#ifndef CHUNK_TESTS_H
#define CHUNK_TESTS_H

#include <gtest/gtest.h>
#include "printer.hpp"
#include "../chunk.hpp"

//Use a mock chunk class to test the chunk buffer
class DummyChunk : public Chunk {
public:
  DummyChunk(int xIndex, int yIndex, int zIndex)
  : Chunk(xIndex, yIndex, zIndex) { }
  
protected:
  virtual void generateData()
  {
    Chunk::generateData();
  }
};

TEST(ChunkTests, WorldToChunkCoordsTest1) {
  glm::vec3 playerPos(1.0, 1.0, -1.0);
  EXPECT_EQ(glm::ivec3(0, 0, 0), Chunk::worldToChunkSpace(playerPos));
}

TEST(ChunkTests, WorldToChunkCoordsTest2) {
  const GLfloat VOXEL_SIZE = Units::voxelToGl(1);
  glm::vec3 playerPos = glm::vec3(0.1, 0.0, -(VOXEL_SIZE * VOXELS_PER_CHUNK + (VOXEL_SIZE * .5)));
  EXPECT_EQ(glm::ivec3(0, 0, 1), Chunk::worldToChunkSpace(playerPos));
}

TEST(ChunkTests, BufferFinishClearsLoadQueue) {
  ChunkBuffer<DummyChunk> buf;
  buf.init();
  buf.finishLoadingChunks();
  EXPECT_EQ(0, buf.getLoadQueueSize());
  
  Events::playerMoveEvent(glm::vec4(100, 100, 100, 1), glm::vec4(200, 200, 200, 1));
  buf.finishLoadingChunks();
  EXPECT_EQ(0, buf.getLoadQueueSize());
}

//Make sure we load the correct number of chunk on initialization
TEST(ChunkTests, BufferMaintainsProperSize) {
  ChunkBuffer<DummyChunk> buf;
  buf.init();
  
  //Simulate a player move
  Events::playerMoveEvent(glm::vec4(0, 0, 0, 1), glm::vec4(0, 0, -1, 1));
  buf.finishLoadingChunks();
  EXPECT_EQ(VISIBLE_CHUNKS * VISIBLE_CHUNKS, buf.getRenderQueueSize());
  
  //Move the player around to random locations, make sure the
  //size of the buffer never changes
  Events::playerMoveEvent(glm::vec4(0, 0, -1, 1), glm::vec4(1, 1, 1, 1));
  buf.finishLoadingChunks();
  EXPECT_EQ(VISIBLE_CHUNKS * VISIBLE_CHUNKS, buf.getRenderQueueSize());
  
  Events::playerMoveEvent(glm::vec4(1, 1, 1, 1), glm::vec4(200, 200, -200, 1));
  buf.finishLoadingChunks();
  EXPECT_EQ(VISIBLE_CHUNKS * VISIBLE_CHUNKS, buf.getRenderQueueSize());
  
  Events::playerMoveEvent(glm::vec4(1, 1, 1, 1), glm::vec4(5000, 0, -10000, 1));
  buf.finishLoadingChunks();
  EXPECT_EQ(VISIBLE_CHUNKS * VISIBLE_CHUNKS, buf.getRenderQueueSize());
}

TEST(ChunkTests, ChunkReturnsCorrectMinVoxelCoords) {
  DummyChunk ch1(3, 3, -3);
  glm::ivec3 minvc = ch1.getMinimumAbsVoxelCoord();
  EXPECT_EQ(glm::ivec3(3 * VOXELS_PER_CHUNK, 3 * VOXELS_PER_CHUNK, -3 * VOXELS_PER_CHUNK), minvc);
}

TEST(ChunkTests, InterpolatesNoiseInputsCorrectly) {
  DummyChunk ch1(0, 0, 0);
  glm::vec3 v000(ch1.getNoiseModuleInput(glm::vec3(0, 0, 0)));
  glm::vec3 v100(ch1.getNoiseModuleInput(glm::vec3(1, 0, 0)));
  glm::vec3 v200(ch1.getNoiseModuleInput(glm::vec3(2, 0, 0)));
  
  EXPECT_EQ(glm::vec3(0.0, 0.0, 0.0), v000);
  EXPECT_EQ(glm::vec3(.125, 0.0, 0.0), v100);
  EXPECT_EQ(glm::vec3(.25, 0.0, 0.0), v200);
}

TEST(ChunkTests, InterpolatesNoiseInputsForSeams) {
  DummyChunk cha(0, 0, 0);
  DummyChunk chb(1, 0, 0);
  glm::vec3 a300(cha.getNoiseModuleInput(glm::vec3(7, 0, 0)));
  glm::vec3 b000(chb.getNoiseModuleInput(glm::vec3(0, 0, 0)));
  
  EXPECT_EQ(a300, b000);
}

TEST(ChunkTests, HeightValuesMatchAcrossChunks) {
  DummyChunk ch1(0, 0, 0);
  DummyChunk ch2(1, 0, 0);
  DummyChunk ch3(0, 0, 1);
  
  ch1.generate();
  ch2.generate();
  ch3.generate();
  
  EXPECT_EQ(ch1.heightAt(VOXELS_PER_CHUNK - 1, 0), ch2.heightAt(0, 0));
}













#endif