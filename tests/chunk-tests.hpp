#ifndef CHUNK_TESTS_H
#define CHUNK_TESTS_H

#include <gtest/gtest.h>
#include "printer.hpp"

TEST(ChunkTests, WorldToChunkCoordsTest1) {
  glm::vec3 playerPos(1.0, 1.0, -1.0);
  EXPECT_EQ(glm::ivec3(0, 0, 0), Chunk::worldToChunkSpace(playerPos));
  
  playerPos = glm::vec3(0.1, 0.0, -(VOXEL_SIZE + (VOXEL_SIZE * .5)));
  EXPECT_EQ(glm::ivec3(0, 0, 1), Chunk::worldToChunkSpace(playerPos));
}

#endif