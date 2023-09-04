#include "chunkData.hpp"

#include <iostream>

ChunkData::ChunkData() {
  std::fill(&blocks[0][0][0], &blocks[0][0][0] + BLOCKS_TOTAL, BlockType::NONE);
}

ChunkData::~ChunkData() {
}


BlockType ChunkData::get(int x, int y, int z) {
  return blocks[x][y][z];
}

void ChunkData::set(int x, int y, int z, BlockType block) {
  blocks[x][y][z] = block;
}
