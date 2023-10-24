#pragma once

#include <queue>
#include <tuple>
#include <unordered_set>
#include "../../common/chunkData.hpp"
#include "../../common/world.hpp"

class ChunkMeshBuildQueue {
public:
  ChunkMeshBuildQueue() {}
  ~ChunkMeshBuildQueue() {}

  void updateBlock(int x, int y, int z);
  void updateChunk(int ox, int oy, int oz);

  std::tuple<int,int,int> pop();
  bool isEmpty();

private:
  std::queue<std::tuple<int,int,int>> queue;
  std::unordered_set<ChunkKeyType> isInQueue;

  void push(int ox, int oy, int oz);
};
