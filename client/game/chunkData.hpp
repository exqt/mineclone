#pragma once

#include <array>
#include <memory>
#include "block.hpp"

class ChunkData {
public:
  static const int BLOCKS_X = 16;
  static const int BLOCKS_Y = 128;
  static const int BLOCKS_Z = 16;
  static const int BLOCKS_TOTAL = BLOCKS_X * BLOCKS_Y * BLOCKS_Z;

  ChunkData();
  ~ChunkData();

  BlockType get(int x, int y, int z);
  void set(int x, int y, int z, BlockType block);

private:
  BlockType blocks[BLOCKS_X][BLOCKS_Y][BLOCKS_Z];
};

using ChunkDataPtr = std::shared_ptr<ChunkData>;
