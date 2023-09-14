#include "chunkData.hpp"

#include <iostream>

ChunkData::ChunkData() {
}

ChunkData::~ChunkData() {
}


BlockType ChunkData::get(int x, int y, int z) {
  return blocks[x][y][z];
}

void ChunkData::set(int x, int y, int z, BlockType block) {
  blocks[x][y][z] = block;
}
