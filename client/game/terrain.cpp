#include "terrain.hpp"

#include <FastNoise/FastNoise.h>

#include <iostream>
#include <vector>
#include <cstring>
#include <utility>

int mapIndex(int x, int y, int width) {
  return y * width + x;
}

int mapIndex(int x, int y, int z, int width, int height) {
  return z * width * height + y * width + x;
}

Terrain::Terrain(int seed) : seed(seed)
{
}

Terrain::~Terrain()
{
}

std::vector<float> Terrain::getHeightData(int ox, int oz) {
  auto fnSimplex = FastNoise::New<FastNoise::Simplex>();
  auto fnFractal = FastNoise::New<FastNoise::FractalFBm>();
  auto fnRemap = FastNoise::New<FastNoise::Remap>();

  fnFractal->SetSource( fnSimplex );
  fnFractal->SetOctaveCount( 5 );

  fnRemap->SetSource( fnFractal );
  fnRemap->SetRemap(-1.0f, 1.0f, 0.0f, 50.0f);

  std::vector<float> heights(ChunkData::BLOCKS_X * ChunkData::BLOCKS_Z);
  fnRemap->GenUniformGrid2D(
    heights.data(),
    ox * ChunkData::BLOCKS_X,
    oz * ChunkData::BLOCKS_Z,
    ChunkData::BLOCKS_X,
    ChunkData::BLOCKS_Z,
    0.005f,
    seed
  );

  return heights;
}

std::vector<float> Terrain::getCaveData(int ox, int oy, int oz) {
  // change it to cellular noise
  auto fnSimplex = FastNoise::New<FastNoise::Perlin>();
  auto fnFractal = FastNoise::New<FastNoise::FractalFBm>();

  fnFractal->SetSource( fnSimplex );
  fnFractal->SetOctaveCount( 5 );

  std::vector<float> caves(ChunkData::BLOCKS_X * ChunkData::BLOCKS_Y * ChunkData::BLOCKS_Z);
  fnFractal->GenUniformGrid3D(
    caves.data(),
    ox * ChunkData::BLOCKS_X,
    oy * ChunkData::BLOCKS_Y,
    oz * ChunkData::BLOCKS_Z,
    ChunkData::BLOCKS_X,
    ChunkData::BLOCKS_Y,
    ChunkData::BLOCKS_Z,
    0.04f,
    seed
  );

  return caves;
}

#include <chrono>

ChunkDataPtr Terrain::getChunkData(int ox, int oy, int oz) {
  ChunkDataPtr chunkData = std::make_shared<ChunkData>();
  if (oy == 1) return chunkData;

  auto heights = getHeightData(ox, oz);
  // auto caves = getCaveData(ox, oy, oz);

  const int SEA_HEIGHT = 30;

  for (int ix = 0; ix < ChunkData::BLOCKS_X; ix++) {
    for (int iz = 0; iz < ChunkData::BLOCKS_Z; iz++) {
      int h = heights[mapIndex(ix, iz, ChunkData::BLOCKS_X)];
      int nDirts = h / 10;

      for (int iy = 0; iy < ChunkData::BLOCKS_Y; iy++) {
        int y = iy + oy*ChunkData::BLOCKS_Y;
        if (y == 0) {
          chunkData->set(ix, iy, iz, BEDROCK);
          continue;
        }

        if (h < SEA_HEIGHT) {
          if (y < h - nDirts) {
            chunkData->set(ix, iy, iz, STONE);
          }
          else if (y <= h) {
            chunkData->set(ix, iy, iz, SAND);
          }
          else if (y <= SEA_HEIGHT) {
            chunkData->set(ix, iy, iz, WATER);
          }
          else {
            chunkData->set(ix, iy, iz, NONE);
          }
        }
        else {
          if (y < h - nDirts) {
            chunkData->set(ix, iy, iz, STONE);
          }
          else if (y < h) {
            chunkData->set(ix, iy, iz, DIRT);
          }
          else if (y == h) {
            chunkData->set(ix, iy, iz, DIRT_GRASS);
          }
          else {
            chunkData->set(ix, iy, iz, NONE);
          }
        }
      }
    }
  }

  int treePosDisplacement = hash(ox, oz);
  for (int ix = 2; ix < 16 - 2; ix += 5) {
    for (int iz = 2; iz < 16 - 2; iz += 5) {
      int dx = treePosDisplacement % 3 - 1;
      treePosDisplacement /= 3;
      int dz = treePosDisplacement % 3 - 1;
      treePosDisplacement /= 3;
      int x = ix + dx;
      int z = iz + dz;
      int h = heights[mapIndex(x, z, ChunkData::BLOCKS_X)];
      if (h < SEA_HEIGHT) continue;
      plantsTree(chunkData, x, h, z);
    }
  }

  return chunkData;
}

void Terrain::plantsTree(ChunkDataPtr chunkData, int x, int y, int z) {
  int px = x - 2;
  int py = y;
  int pz = z - 2;
  int qx = x + 2;
  int qy = y + 8;
  int qz = z + 2;

  if (!(0 <= px && qx < ChunkData::BLOCKS_X &&
      0 <= py && qy < ChunkData::BLOCKS_Y &&
      0 <= pz && qz < ChunkData::BLOCKS_Z)) return;


  for (int iy = y + 6; iy <= y + 8; iy++) {
    int r = 2;
    if (iy == y + 8) r = 1;
    for (int ix = x - r; ix <= x + r; ix++) {
      for (int iz = z - r; iz <= z + r; iz++) {
        chunkData->set(ix, iy, iz, TREE_LEAVES);
      }
    }
  }

  for (int iy = y; iy < y + 7; iy++) {
    chunkData->set(x, iy, z, TREE_TRUNK);
  }
}


// very poor hash don't use it
int Terrain::hash(int x, int z) {
  auto h = 0xdeadbeef;
  h ^= x * 0x9e3779b9;
  h ^= z * 0x1f3a98bc + (h << 19) + (h >> 15);
  h ^= x * 0x86ab1cef + (h << 13) + (h >> 16);
  h ^= z * 0x7fa3bc03 + (h << 7) + (h >> 14);
  return h & 0x7FFFFFFF;
}
