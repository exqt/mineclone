#pragma once

#include <vector>
#include <map>
#include <tuple>
#include <queue>
#include <unordered_map>

#include <glm/glm.hpp>

#include "terrain.hpp"
#include "block.hpp"
#include "chunkData.hpp"

typedef unsigned long long int ChunkKeyType;

// chunkKey : 2^30 2^30 2^4
// -2^29 <= x < 2^29


class World {
public:
  static const int MAX_HEIGHT = 128;
  static const int MAX_CHUNKS_Y = MAX_HEIGHT / ChunkData::BLOCKS_Y - 1;

  World();
  ~World();

  Terrain* terrain;

  ChunkDataPtr getChunkData(int ox, int oy, int oz);
  ChunkDataPtr getChunkData(std::tuple<int,int,int> id);
  BlockType getBlock(int x, int y, int z);
  void placeBlock(int x, int y, int z, BlockType block);

  void update(glm::vec3 playerPos);

  std::queue<std::tuple<int,int,int>>& getRenderQueue();
  std::queue<std::tuple<int,int,int>>& getUnloadQueue();

  ChunkKeyType toChunkKey(int x, int y, int z);
  std::tuple<int,int,int> getChunkId(ChunkKeyType key);

private:
  std::unordered_map<ChunkKeyType, ChunkDataPtr> chunks;
  std::unordered_map<ChunkKeyType, unsigned char> adjacentLoadedCount;
  std::queue<std::tuple<int,int,int>> chunkLoadQueue, renderQueue, unloadQueue;

  void loadChunkInside(int x, int y, int z, int radius);
  void unloadChunkOutside(int x, int y, int z, int radius);
};
