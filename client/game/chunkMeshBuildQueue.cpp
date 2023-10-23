#include "chunkMeshBuildQueue.hpp"

void ChunkMeshBuildQueue::updateBlock(int x, int y, int z) {
  int ox = x / ChunkData::BLOCKS_X;
  int oy = y / ChunkData::BLOCKS_Y;
  int oz = z / ChunkData::BLOCKS_Z;

  if (x % ChunkData::BLOCKS_X == 0) push(ox - 1, oy, oz);
  if (x % ChunkData::BLOCKS_X == ChunkData::BLOCKS_X - 1) push(ox + 1, oy, oz);

  if (z % ChunkData::BLOCKS_Z == 0) push(ox, oy, oz - 1);
  if (z % ChunkData::BLOCKS_Z == ChunkData::BLOCKS_Z - 1) push(ox, oy, oz + 1);

  if (x % ChunkData::BLOCKS_X == 0 && z % ChunkData::BLOCKS_Z == 0) push(ox - 1, oy, oz - 1);
  if (x % ChunkData::BLOCKS_X == ChunkData::BLOCKS_X - 1 && z % ChunkData::BLOCKS_Z == 0) push(ox + 1, oy, oz - 1);
  if (x % ChunkData::BLOCKS_X == 0 && z % ChunkData::BLOCKS_Z == ChunkData::BLOCKS_Z - 1) push(ox - 1, oy, oz + 1);
  if (x % ChunkData::BLOCKS_X == ChunkData::BLOCKS_X - 1 && z % ChunkData::BLOCKS_Z == ChunkData::BLOCKS_Z - 1) push(ox + 1, oy, oz + 1);
}

void ChunkMeshBuildQueue::updateChunk(int ox, int oy, int oz) {
  for (int ix = -1; ix <= 1; ix++) {
    for (int iz = -1; iz <= 1; iz++) {
      push(ox + ix, oy, oz + iz);
    }
  }
}

void ChunkMeshBuildQueue::push(int ox, int oy, int oz) {
  ChunkKeyType key = World::toChunkKey(ox, oy, oz);
  if (isInQueue.find(key) == isInQueue.end()) {
    queue.push(std::make_tuple(ox, oy, oz));
    isInQueue.insert(key);
  }
}

std::tuple<int,int,int> ChunkMeshBuildQueue::pop() {
  auto ret = queue.front();
  queue.pop();
  isInQueue.erase(World::toChunkKey(std::get<0>(ret), std::get<1>(ret), std::get<2>(ret)));
  return ret;
}

bool ChunkMeshBuildQueue::isEmpty() {
  return queue.empty();
}
