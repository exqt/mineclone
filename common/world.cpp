#include "world.hpp"

#include <iostream>
#include <cassert>
#include <algorithm>

namespace {
  int idiv(int x, int y) {
    return x >= 0 ? x/y : -((-x - 1) / y + 1);
  }
}

World::World() {
}

World::~World() {

}

ChunkDataPtr World::getChunkData(int ox, int oy, int oz) {
  auto it = chunks.find(toChunkKey(ox, oy, oz));
  if (it != chunks.end()) return it->second;

  auto chunkData = chunkLoader(ox, oy, oz);
  chunks[toChunkKey(ox, oy, oz)] = chunkData;
  return chunkData;
}

void World::setChunkData(int ox, int oy, int oz, ChunkDataPtr data) {
  chunks[toChunkKey(ox, oy, oz)] = data;
}

BlockType World::getBlock(int x, int y, int z) {
  int cx = idiv(x, ChunkData::BLOCKS_X);
  int cy = idiv(y, ChunkData::BLOCKS_Y);
  int cz = idiv(z, ChunkData::BLOCKS_Z);

  auto key = toChunkKey(cx, cy, cz);
  auto it = chunks.find(key);
  if (it != chunks.end() && it->second != nullptr) {
    return it->second->get(
      x-cx*ChunkData::BLOCKS_X,
      y-cy*ChunkData::BLOCKS_Y,
      z-cz*ChunkData::BLOCKS_Z
    );
  } else {
    return BlockType::NOT_LOADED;
  }
}

void World::placeBlock(int x, int y, int z, BlockType block) {
  int cx = idiv(x, ChunkData::BLOCKS_X);
  int cy = idiv(y, ChunkData::BLOCKS_Y);
  int cz = idiv(z, ChunkData::BLOCKS_Z);

  auto chunkData = getChunkData(cx, cy, cz);
  int lx = x - cx * ChunkData::BLOCKS_X;
  int ly = y - cy * ChunkData::BLOCKS_Y;
  int lz = z - cz * ChunkData::BLOCKS_Z;
  chunkData->set(lx, ly, lz, block);
}

bool World::isLoadedChunk(int ox, int oy, int oz) {
  auto key = toChunkKey(ox, oy, oz);
  auto it = chunks.find(key);
  return it != chunks.end();
}

ChunkKeyType World::toChunkKey(int x, int y, int z) {
  // -2^29 <= x < 2^29 // 30 bits
  // 0 <= y < 16 // 4 bits
  // -2^29 <= z < 2^29 // 30 bits
  x += 1 << 29;
  z += 1 << 29;
  return ((ChunkKeyType)x << 34) | ((ChunkKeyType)y << 30) | (ChunkKeyType)z;
}

std::tuple<int,int,int> World::getChunkId(ChunkKeyType key) {
  int x = ((key >> 34) & 0x3FFFFFFFu) - (1 << 29);
  int y = ((key >> 30) & 0b1111u);
  int z = (key & 0x3FFFFFFFu) - (1 << 29);
  return std::tuple<int,int,int>(x, y, z);
}

