#pragma once

#include <SDL2/SDL.h>

#include <glm/glm.hpp>

#include <vector>
#include <queue>

#include "core/input.hpp"
#include "core/camera.hpp"
#include "core/graphics/font.hpp"
#include "core/graphics/frameBuffer.hpp"
#include "core/object.hpp"
#include "config.hpp"

#include "game/objects/player.hpp"
#include "game/skyDome.hpp"
#include "game/worldLoader.hpp"

#include "application.hpp"
#include "game/gameState.hpp"
#include "game/objects/chunkObject.hpp"

#include "../common/world.hpp"
#include "../common/chunkData.hpp"
#include "../common/gamePacket.hpp"
#include "../common/byteStream.hpp"

class Game {
public:
  double time;

  PerspectiveCamera* camera;
  Config config;
  Player* player;

  World* world;
  WorldLoader* worldLoader;

  CollisionMapPtr collisionMap;

  FramebufferPtr mainBuffer, waterBuffer, skyBuffer;
  MeshPtr fullscreenQuad;

  Lines* crosshair;
  SkyDome* skyDome;

  Game();
  ~Game();

  void update(float dt);
  void draw();

  void setGameSize(int width, int height);

  void onRPCResponse(std::string name, DataReadStream& stream);
  void onObjectSync(ObjectId id, DataReadStream& stream);

private:
  int width, height;
  std::vector<Object*> objects;
  std::map<ChunkKeyType, ChunkObject*> chunkObjects;

  std::queue<ChunkObject*> meshBuildQueue;

  float updateTime = 0, renderTime = 0;

  void processChunks();
};
