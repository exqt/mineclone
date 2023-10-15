#pragma once

#include <vector>
#include <map>
#include <tuple>
#include <queue>
#include <unordered_map>
#include <functional>

#include "../common/block.hpp"
#include "../common/chunkData.hpp"

typedef unsigned long long int ChunkKeyType;

// chunkKey : 2^30 2^30 2^4
// -2^29 <= x < 2^29

class World {
public:
  static const int MAX_HEIGHT = 128;
  static const int MAX_CHUNKS_Y = MAX_HEIGHT / ChunkData::BLOCKS_Y - 1;

  World();
  ~World();

  ChunkDataPtr getChunkData(int ox, int oy, int oz);
  void setChunkData(int ox, int oy, int oz, ChunkDataPtr data);

  BlockType getBlock(int x, int y, int z);
  void placeBlock(int x, int y, int z, BlockType block);

  bool isLoadedChunk(int ox, int oy, int oz);

  static ChunkKeyType toChunkKey(int x, int y, int z);
  static std::tuple<int,int,int> getChunkId(ChunkKeyType key);

  std::function<ChunkDataPtr(int x, int y, int z)> chunkLoader;

private:
  std::unordered_map<ChunkKeyType, ChunkDataPtr> chunks;
};
