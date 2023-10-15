#pragma once

#include "../../common/chunkData.hpp"
#include "../../common/world.hpp"
#include <functional>
#include <queue>
#include <set>

class WorldLoader {
public:
  WorldLoader();
  World* world;

  void requestRenderBlock(int x, int y, int z);
  void requestRenderChunk(int ox, int oy, int oz);
  void requestAroundPlayer(float x, float y, float z);

  void onRetrieveChunkData(int ox, int oy, int oz, ChunkDataPtr data);
  std::tuple<int, int, int> getChunkIdToRender();
  bool hasChunksToRender();

private:
  std::queue<std::tuple<int,int,int>> renderQueue;
  std::set<std::tuple<int,int,int>> inRenderQueue;

  void loadChunkInside(int x, int y, int z, int radius);
  void unloadChunkOutside(int x, int y, int z, int radius);
};
