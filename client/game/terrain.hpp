#pragma once

#include "block.hpp"
#include <array>
#include <vector>
#include <unordered_map>
#include "chunkData.hpp"

using vectorf = std::vector<float>;

struct HashTuple {
  template <class T1, class T2>
  std::size_t operator()(const std::tuple<T1, T2>& tuple) const {
    auto [x, y] = tuple;
    return (x << 16) ^ y;
  }
};

struct EqualTuple {
  template <class T1, class T2>
  bool operator()(const std::tuple<T1, T2>& lhs, const std::tuple<T1, T2>& rhs) const {
    return lhs == rhs;
  }
};

class Terrain {
public:
  Terrain(int seed);
  ~Terrain();

  vectorf getHeightData(int ox, int oz);
  vectorf getCaveData(int ox, int oy, int oz);

  ChunkDataPtr getChunkData(int ox, int oy, int oz);

private:
  int seed;
  std::unordered_map<std::tuple<int,int>, vectorf, HashTuple, EqualTuple> heightMapCache;
};
