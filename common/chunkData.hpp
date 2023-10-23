#pragma once

#include <array>
#include <memory>
#include "block.hpp"

class ChunkData;
using ChunkDataPtr = std::shared_ptr<ChunkData>;

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

  std::vector<std::byte> toByteArray();
  void copyFromByteArray(std::vector<std::byte> data);

private:
  BlockType blocks[BLOCKS_X][BLOCKS_Y][BLOCKS_Z];
};
