#pragma once

#include <SDL2/SDL.h>

#include <glm/glm.hpp>

#include <vector>
#include <queue>
#include <tuple>

#include "core/input.hpp"
#include "core/graphics/camera.hpp"
#include "core/graphics/font.hpp"
#include "core/graphics/frameBuffer.hpp"
#include "core/object.hpp"
#include "config.hpp"

#include "game/objects/player.hpp"
#include "game/skyDome.hpp"

#include "application.hpp"
#include "game/gameState.hpp"
#include "game/objects/chunkObject.hpp"
#include "game/chunkMeshBuildQueue.hpp"

#include "../common/world.hpp"
#include "../common/chunkData.hpp"
#include "../common/byteStream.hpp"
#include "../common/networkObject.hpp"

class Game {
public:
  double time;

  PerspectiveCamera* camera;
  Config config;
  MyPlayer* player;

  World* world;

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

  void syncOwnedObjects();
  void onRPC(DataReadStream& stream);

private:
  NetworkObjectId userId = -1;

  int width, height;
  std::map<NetworkObjectId, Object*> objects, ownedObjects;
  std::map<ChunkKeyType, ChunkObject*> chunkObjects;
  ChunkMeshBuildQueue meshBuildQueue;

  float updateTime = 0, renderTime = 0;

  void processChunks();

  std::map<std::string, std::function<void(NetworkObjectData)>> objectCreators;
  void registerObjects();
};
