#include "game.hpp"


Game::Game() {
  world = new World();
  terrain = new Terrain(1337);

  world->chunkLoader = [&](int x, int y, int z) {
    return terrain->getChunkData(x, y, z);
  };
}

Game::~Game() {

}

#include <iostream>


// input : x, y, z, block
DataWriteStream Game::setBlock(DataReadStream& stream) {
  auto x = stream.pop<int>();
  auto y = stream.pop<int>();
  auto z = stream.pop<int>();
  auto block = stream.pop<BlockType>();

  std::cout << "setBlock " << x << " " << y << " " << z << " " << block << std::endl;

  world->placeBlock(x, y, z, block);

  auto ret = DataWriteStream();
  ret.push<int>(1);

  return ret;
}

// input : x, y, z
// output : data
DataWriteStream Game::getChunkData(DataReadStream& stream) {
  auto x = stream.pop<int>();
  auto y = stream.pop<int>();
  auto z = stream.pop<int>();

  DataWriteStream ret;
  ret.push<int>(x);
  ret.push<int>(y);
  ret.push<int>(z);
  ret.pushVector(world->getChunkData(x, y, z)->toByteArray());

  return ret;
}
