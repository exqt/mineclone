#include "chunkData.hpp"

#include <iostream>
#include <cassert>

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

ChunkDataPtr ChunkData::fromByteArray(std::vector<std::byte> data) {
  std::vector<std::byte> flatten;

  for (size_t i = 0; i < data.size(); i++) {
    if (data[i] == (std::byte)BlockType::COMPRESSING_BYTE) {
      int cnt = (int)data[i + 1];
      std::byte block = data[i + 2];

      for (int j = 0; j < cnt; j++) {
        flatten.push_back(block);
      }

      i += 2;
    } else {
      flatten.push_back(data[i]);
    }
  }

  if (flatten.size() != BLOCKS_TOTAL) {
    std::cerr << "ChunkData::fromByteArray: flatten.size() != BLOCKS_TOTAL" << std::endl;
    std::exit(1);
  }

  ChunkDataPtr ret = std::make_shared<ChunkData>();
  std::copy(flatten.begin(), flatten.end(), (std::byte*)ret->blocks);

  return ret;
}

ChunkDataPtr ChunkData::createEmpty() {
  ChunkDataPtr ret = std::make_shared<ChunkData>();
  std::fill((std::byte*)ret->blocks, (std::byte*)ret->blocks + sizeof(ret->blocks), (std::byte)BlockType::NONE);
  return ret;
}
