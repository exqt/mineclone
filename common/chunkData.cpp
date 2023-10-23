#include "chunkData.hpp"

#include <iostream>
#include <cassert>

ChunkData::ChunkData() {
  std::fill(blocks[0][0], blocks[0][0] + BLOCKS_TOTAL, BlockType::NONE);
}

ChunkData::~ChunkData() {
}


BlockType ChunkData::get(int x, int y, int z) {
  return blocks[x][y][z];
}

void ChunkData::set(int x, int y, int z, BlockType block) {
  blocks[x][y][z] = block;
}

// with run-length encoding
std::vector<std::byte> ChunkData::toByteArray() {
  std::vector<std::byte> flatten, ret;
  flatten.insert(flatten.end(), (std::byte*)blocks, (std::byte*)blocks + sizeof(blocks));

  std::byte last = flatten[0];
  int cnt = 1;

  auto push = [&]() {
    if (cnt > 4) {
      ret.push_back((std::byte)BlockType::COMPRESSING_BYTE);
      ret.push_back((std::byte)cnt);
      ret.push_back((std::byte)last);
    }
    else {
      for (int i = 0; i < cnt; i++) {
        ret.push_back((std::byte)last);
      }
    }
  };

  for (int i = 1; i < BLOCKS_TOTAL; i++) {
    if (last == flatten[i] && cnt < 255) {
      cnt++;
    } else {
      push();
      last = flatten[i];
      cnt = 1;
    }
  }

  push();

  return ret;
}

void ChunkData::copyFromByteArray(std::vector<std::byte> data) {
  int count = 0;
  BlockType* blocks = (BlockType*)this->blocks;

  for (size_t i = 0; i < data.size(); i++) {
    if (data[i] == (std::byte)BlockType::COMPRESSING_BYTE) {
      int cnt = (int)data[i + 1];
      std::byte block = data[i + 2];

      for (int j = 0; j < cnt; j++) {
        blocks[count] = (BlockType)block;
        count++;
      }

      i += 2;
    } else {
      blocks[count] = (BlockType)data[i];
      count++;
    }
  }

  if (count != BLOCKS_TOTAL) {
    std::cerr << "count != BLOCKS_TOTAL" << std::endl;
    std::exit(1);
  }
}
