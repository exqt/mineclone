#include "worldLoader.hpp"
#include "../core/managers/networkManager.hpp"
#include <iostream>

namespace {
  int idiv(int x, int y) {
    return x >= 0 ? x/y : -((-x - 1) / y + 1);
  }
}

WorldLoader::WorldLoader() {
}

void WorldLoader::requestRenderBlock(int x, int y, int z) {
  int cx = idiv(x, ChunkData::BLOCKS_X);
  int cy = idiv(y, ChunkData::BLOCKS_Y);
  int cz = idiv(z, ChunkData::BLOCKS_Z);

  std::vector<int> adjX = {0}, adjZ = {0};
  if (x % ChunkData::BLOCKS_X == 0) adjX.push_back(-1);
  if (x % ChunkData::BLOCKS_X == ChunkData::BLOCKS_X - 1) adjX.push_back(1);

  if (z % ChunkData::BLOCKS_Z == 0) adjZ.push_back(-1);
  if (z % ChunkData::BLOCKS_Z == ChunkData::BLOCKS_Z - 1) adjZ.push_back(1);

  for (int ix : adjX) {
    for (int iz : adjZ) {
      requestRenderChunk(cx + ix, cy, cz + iz);
    }
  }
}

void WorldLoader::requestRenderChunk(int ox, int oy, int oz) {
  if (!world->isLoadedChunk(ox, oy, oz)) return;
  std::tuple<int,int,int> chunkId(ox, oy, oz);
  if (inRenderQueue.find(chunkId) == inRenderQueue.end()) {
    renderQueue.push(chunkId);
    inRenderQueue.insert(chunkId);
  }
}

void WorldLoader::requestAroundPlayer(float x, float y, float z) {
  loadChunkInside(x, y, z, 128);
  // TODO: unload chunks outside
  // unloadChunkOutside(x, y, z, 160);
}

void WorldLoader::onRetrieveChunkData(int ox, int oy, int oz, ChunkDataPtr data) {
  world->setChunkData(ox, oy, oz, data);

  for (int ix = -1; ix <= 1; ix++) {
    for (int iz = -1; iz <= 1; iz++) {
      requestRenderChunk(ox + ix, oy, oz + iz);
    }
  }
}

void WorldLoader::loadChunkInside(int x, int y, int z, int radius) {
  int px = idiv(x, ChunkData::BLOCKS_X);
  int py = idiv(y, ChunkData::BLOCKS_Y);
  int pz = idiv(z, ChunkData::BLOCKS_Z);
  int cx = idiv(x - radius, ChunkData::BLOCKS_X);
  int cy = 0; // idiv(y - radius, ChunkData::BLOCKS_Y);
  int cz = idiv(z - radius, ChunkData::BLOCKS_Z);
  int dx = idiv(x + radius, ChunkData::BLOCKS_X);
  int dy = World::MAX_HEIGHT / ChunkData::BLOCKS_Y - 1;  // idiv(y + radius, ChunkData::BLOCKS_Y);
  int dz = idiv(z + radius, ChunkData::BLOCKS_Z);

  std::vector<std::tuple<int,int,int>> toAdd;

  for (int ix = cx; ix <= dx; ix++) {
    for (int iy = cy; iy <= dy; iy++) {
      for (int iz = cz; iz <= dz; iz++) {
        if (!world->isLoadedChunk(ix, iy, iz)) {
          auto emptyChunk = std::make_shared<ChunkData>();
          world->setChunkData(ix, iy, iz, emptyChunk);
          toAdd.push_back(std::tuple<int,int,int>(ix, iy, iz));
        }
      }
    }
  }

  std::sort(toAdd.begin(), toAdd.end(), [px, py, pz](auto a, auto b) {
    auto [ax, ay, az] = a;
    auto [bx, by, bz] = b;
    int distA = std::abs(ax-px) + std::abs(ay-py) + std::abs(az-pz);
    int distB = std::abs(bx-px) + std::abs(by-py) + std::abs(bz-pz);
    return distA < distB;
  });

  auto networkManager = NetworkManager::Instance();

  for (auto id : toAdd) {
    auto [ox, oy, oz] = id;
    DataWriteStream stream;
    stream.push(ox);
    stream.push(oy);
    stream.push(oz);
    networkManager.rpcCall("getChunkData", stream);
  }
}

void WorldLoader::unloadChunkOutside(int x, int y, int z, int radius) {
  /*
  int cx = idiv(x - radius, ChunkData::BLOCKS_X);
  int cy = 0; // idiv(y - radius, ChunkData::BLOCKS_Y);
  int cz = idiv(z - radius, ChunkData::BLOCKS_Z);
  int dx = idiv(x + radius, ChunkData::BLOCKS_X);
  int dy = MAX_HEIGHT / ChunkData::BLOCKS_Y - 1;  // idiv(y + radius, ChunkData::BLOCKS_Y);
  int dz = idiv(z + radius, ChunkData::BLOCKS_Z);

  std::vector<std::tuple<int,int,int>> toUnload;
  for (auto it : chunks) {
    auto [ix, iy, iz] = getChunkId(it.first);
    if (cx <= ix && ix <= dx && cy <= iy && iy <= dy && cz <= iz && iz <= dz) {
      continue;
    }

    toUnload.push_back(std::tuple<int,int,int>(ix, iy, iz));
  }

  for (auto id : toUnload) {
    auto [ox, oy, oz] = id;
    auto chunkKey = toChunkKey(ox, oy, oz);
    auto chunkData = chunks[chunkKey];
    chunks.erase(chunkKey);
    unloadQueue.push(id);

    for (int ix = -1; ix <= 1; ix++) {
      for (int iy = -1; iy <= 1; iy++) {
        for (int iz = -1; iz <= 1; iz++) {
          int y = oy + iy;
          if (!(0 <= y && y <= MAX_CHUNKS_Y)) continue;
          auto adjacentKey = toChunkKey(ox + ix, y, oz + iz);
          adjacentLoadedCount[adjacentKey] -= 1;
        }
      }
    }
  }
  */
}

std::tuple<int, int, int> WorldLoader::getChunkIdToRender() {
  auto chunkId = renderQueue.front(); renderQueue.pop();
  inRenderQueue.erase(chunkId);
  return chunkId;
}

bool WorldLoader::hasChunksToRender() {
  return !renderQueue.empty();
}
