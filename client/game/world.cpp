#include "world.hpp"
#include "../core/util.hpp"

#include <iostream>
#include <cassert>
#include <algorithm>

World::World() {
  terrain = new Terrain(1337);
}

World::~World() {

}

void World::update(glm::vec3 playerPos) {
  loadChunkInside(playerPos.x, playerPos.y, playerPos.z, 64);
  unloadChunkOutside(playerPos.x, playerPos.y, playerPos.z, 128);

  const int maxChunkLoadsPerFrame = 16;
  for (int i = 0; i < maxChunkLoadsPerFrame && !chunkLoadQueue.empty(); i++) {
    auto id = chunkLoadQueue.front(); chunkLoadQueue.pop();
    auto [ox, oy, oz] = id;
    ChunkKeyType key = toChunkKey(ox, oy, oz);
    chunks[key] = terrain->getChunkData(ox, oy, oz);
    // TODO

    for (int ix = -1; ix <= 1; ix++) {
      for (int iy = -1; iy <= 1; iy++) {
        for (int iz = -1; iz <= 1; iz++) {
          int y = oy + iy;
          if (!(0 <= y && y <= MAX_CHUNKS_Y)) continue;
          auto adjacentKey = toChunkKey(ox + ix, y, oz + iz);
          adjacentLoadedCount[adjacentKey] += 1;

          int required = (y == 0 || y == MAX_CHUNKS_Y) ? 18 : 27;
          if (adjacentLoadedCount[adjacentKey] == required) {
            renderQueue.emplace(ox + ix, y, oz + iz);
            if (chunks[toChunkKey(ox + ix, y, oz + iz)] == nullptr) {
              std::cout << "ERROR: adjacent chunk not loaded" << std::endl;
            }
          }
        }
      }
    }
  }
}

ChunkDataPtr World::getChunkData(int ox, int oy, int oz) {
  auto it = chunks.find(toChunkKey(ox, oy, oz));
  if (it != chunks.end()) {
    return it->second;
  } else {
    return nullptr;
  }
}

ChunkDataPtr World::getChunkData(std::tuple<int,int,int> id) {
  auto it = chunks.find(toChunkKey(std::get<0>(id), std::get<1>(id), std::get<2>(id)));
  if (it != chunks.end()) {
    return it->second;
  } else {
    return nullptr;
  }
}

BlockType World::getBlock(int x, int y, int z) {
  int cx = idiv(x, ChunkData::BLOCKS_X);
  int cy = idiv(y, ChunkData::BLOCKS_Y);
  int cz = idiv(z, ChunkData::BLOCKS_Z);

  auto key = toChunkKey(cx, cy, cz);
  auto it = chunks.find(key);
  if (it != chunks.end() && it->second != nullptr) {
    return it->second->get(
      x-cx*ChunkData::BLOCKS_X,
      y-cy*ChunkData::BLOCKS_Y,
      z-cz*ChunkData::BLOCKS_Z
    );
  } else {
    return BlockType::NOT_LOADED;
  }
}

void World::placeBlock(int x, int y, int z, BlockType block) {
  int cx = idiv(x, ChunkData::BLOCKS_X);
  int cy = idiv(y, ChunkData::BLOCKS_Y);
  int cz = idiv(z, ChunkData::BLOCKS_Z);

  auto key = toChunkKey(cx, cy, cz);
  auto it = chunks.find(key);
  if (it != chunks.end()) {
    int lx = x - cx * ChunkData::BLOCKS_X;
    int ly = y - cy * ChunkData::BLOCKS_Y;
    int lz = z - cz * ChunkData::BLOCKS_Z;

    it->second->set(lx, ly, lz, block);
    if (it->second == nullptr) {
      std::cout << "ERROR: chunk is null" << std::endl;
      std::exit(0);
    }

    std::vector<int> dxs = {0}, dys = {0}, dzs = {0};
    if (lx == 0) dxs.push_back(-1);
    if (lx == ChunkData::BLOCKS_X-1) dxs.push_back(1);
    if (ly == 0) dys.push_back(-1);
    if (ly == ChunkData::BLOCKS_Y-1) dys.push_back(1);
    if (lz == 0) dzs.push_back(-1);
    if (lz == ChunkData::BLOCKS_Z-1) dzs.push_back(1);

    for (int dx : dxs) {
      for (int dy : dys) {
        for (int dz : dzs) {
          renderQueue.emplace(cx+dx, cy+dy, cz+dz);
        }
      }
    }
  }
}

void World::loadChunkInside(int x, int y, int z, int radius) {
  int px = idiv(x, ChunkData::BLOCKS_X);
  int py = idiv(y, ChunkData::BLOCKS_Y);
  int pz = idiv(z, ChunkData::BLOCKS_Z);
  int cx = idiv(x - radius, ChunkData::BLOCKS_X);
  int cy = 0; // idiv(y - radius, ChunkData::BLOCKS_Y);
  int cz = idiv(z - radius, ChunkData::BLOCKS_Z);
  int dx = idiv(x + radius, ChunkData::BLOCKS_X);
  int dy = MAX_HEIGHT / ChunkData::BLOCKS_Y - 1;  // idiv(y + radius, ChunkData::BLOCKS_Y);
  int dz = idiv(z + radius, ChunkData::BLOCKS_Z);

  std::vector<std::tuple<int,int,int>> toAdd;

  for (int ix = cx; ix <= dx; ix++) {
    for (int iy = cy; iy <= dy; iy++) {
      for (int iz = cz; iz <= dz; iz++) {
        ChunkKeyType key = toChunkKey(ix, iy, iz);
        if (chunks.count(key) == 0) {
          chunks[key] = nullptr;
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

  for (auto id : toAdd) {
    chunkLoadQueue.push(id);
  }
}

void World::unloadChunkOutside(int x, int y, int z, int radius) {
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
}

std::queue<std::tuple<int,int,int>>& World::getRenderQueue() {
  return renderQueue;
}

std::queue<std::tuple<int,int,int>>& World::getUnloadQueue() {
  return unloadQueue;
}

ChunkKeyType World::toChunkKey(int x, int y, int z) {
  // -2^29 <= x < 2^29 // 30 bits
  // 0 <= y < 16 // 4 bits
  // -2^29 <= z < 2^29 // 30 bits
  x += 1 << 29;
  z += 1 << 29;
  return ((ChunkKeyType)x << 34) | ((ChunkKeyType)y << 30) | (ChunkKeyType)z;
}

std::tuple<int,int,int> World::getChunkId(ChunkKeyType key) {
  int x = ((key >> 34) & 0x3FFFFFFFu) - (1 << 29);
  int y = ((key >> 30) & 0b1111u);
  int z = (key & 0x3FFFFFFFu) - (1 << 29);
  return std::tuple<int,int,int>(x, y, z);
}
