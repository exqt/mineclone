#pragma once

#include "block.hpp"
#include <array>
#include <vector>
#include <unordered_map>
#include "chunkData.hpp"

class Terrain {
public:
  Terrain(int seed);
  ~Terrain();

  std::vector<float> getHeightData(int ox, int oz);
  std::vector<float> getCaveData(int ox, int oy, int oz);

  ChunkDataPtr getChunkData(int ox, int oy, int oz);

private:
  int seed;

  void plantsTree(ChunkDataPtr chunkData, int x, int y, int z);

  int hash(int x, int y);
};
