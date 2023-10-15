#pragma once

#include <vector>
#include <map>

#include "../common/byteStream.hpp"
#include "../common/world.hpp"
#include "../common/block.hpp"
#include "terrain.hpp"

class Game {
public:
  Game();
  ~Game();

  DataWriteStream setBlock(DataReadStream& stream);
  DataWriteStream getChunkData(DataReadStream& stream);

private:
  World* world;
  Terrain* terrain;
};
