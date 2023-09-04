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

vectorf Terrain::getHeightData(int ox, int oz) {
  if (heightMapCache.find(std::make_tuple(ox, oz)) != heightMapCache.end()) {
    return heightMapCache[std::make_tuple(ox, oz)];
  }

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
    0.01f,
    seed
  );

  heightMapCache[std::make_tuple(ox, oz)] = heights;

  return heights;
}

vectorf Terrain::getCaveData(int ox, int oy, int oz) {
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

  auto heights = getHeightData(ox, oz);
  // auto caves = getCaveData(ox, oy, oz);


  for (int ix = 0; ix < ChunkData::BLOCKS_X; ix++) {
    for (int iz = 0; iz < ChunkData::BLOCKS_Z; iz++) {
      int h = heights[mapIndex(ix, iz, ChunkData::BLOCKS_X)];
      int nDirts = h / 10;

      for (int iy = 0; iy < ChunkData::BLOCKS_Y; iy++) {
        // float v = caves[ix][iy][iz];
        // if (v > -0.1f && iy <= h) {
        //   chunkData->set(ix, iy, iz, NONE);
        //   continue;
        // }

        int y = iy + oy*ChunkData::BLOCKS_Y;
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

  return chunkData;
}
