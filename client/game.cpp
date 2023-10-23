#include "game.hpp"

#include "core/managers/assetManager.hpp"
#include "core/input.hpp"

#include "core/managers/networkManager.hpp"

#include <queue>

struct TickMeasure {
  std::queue<Uint64> ticks;
  void tick() {
    auto cur = SDL_GetTicks64();
    ticks.push(cur);
    while (ticks.front() + 1000uLL < cur) { ticks.pop(); }
  }
};

TickMeasure tickMeasure;

#include <iostream>
Game::Game() {
  world = new World();

  collisionMap = std::make_shared<CollisionMap>(world);

  camera = new PerspectiveCamera((float)800/600);
  camera->position = glm::vec3(0.0, 0.0, 3.0);
  camera->far_ = 1000.0;

  crosshair = new Lines();
  crosshair->update({
    4, 0, 0,  -4, 0, 0,
    0, 4, 0,  0, -5, 0,
  }, {
    1, 1, 1,  1, 1, 1,
    1, 1, 1,  1, 1, 1,
  });

  skyDome = new SkyDome();

  fullscreenQuad = std::make_shared<Mesh>();
  fullscreenQuad->update({
    -1.0, -1.0, 0.0,
    1.0, -1.0, 0.0,
    -1.0, 1.0, 0.0,

    1.0, -1.0, 0.0,
    1.0, 1.0, 0.0,
    -1.0, 1.0, 0.0,
  }, {
    0, 0,  1, 0,  0, 1,
    1, 0,  1, 1,  0, 1,
  });

  GameState& gameState = GameState::Instance();
  gameState.setMenuOpen(false);

  registerObjects();
}

Game::~Game() {
}

void Game::update(float dt) {
  Uint64 updateStart = SDL_GetPerformanceCounter();

  Input& input = Input::Instance();
  GameState& gameState = GameState::Instance();
  TickMeasure& tickMeasure = ::tickMeasure;
  tickMeasure.tick();

  if (input.isKeyPressed(config.menu)) {
    gameState.setMenuOpen(!gameState.isMenuOpen());
  }

  processChunks();

  for (auto& [key, chunkObject] : chunkObjects) {
    chunkObject->update(dt);
  }

  for (auto& [id, object] : objects) {
    object->update(dt);
  }

  time = SDL_GetTicks64() / 1000.0;
  updateTime = (double)(SDL_GetPerformanceCounter() - updateStart) / SDL_GetPerformanceFrequency();
}

void Game::draw() {
  Uint64 renderStart = SDL_GetPerformanceCounter();

  DrawContext ctx;
  ctx.camera = camera;
  ctx.width = width;
  ctx.height = height;

  {
    skyBuffer->clear();
    skyBuffer->bind();

    skyDome->setTime(time);
    skyDome->draw(ctx);

    skyBuffer->unbind();
  }

  {
    mainBuffer->clear();
    mainBuffer->bind();

    for (auto& [key, chunkObject] : chunkObjects) {
      chunkObject->draw(ctx);
    }
    for (auto& [id, object] : objects) {
      object->draw(ctx);
    }

    mainBuffer->unbind();
  }

  {
    waterBuffer->clear();
    waterBuffer->bind();
    glDisable(GL_CULL_FACE);

    for (auto &drawCmd : ctx.drawCommands) {
      drawCmd.draw();
    }
    ctx.drawCommands.clear();

    waterBuffer->unbind();

    glEnable(GL_CULL_FACE);
  }

  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mainBuffer->colorTexture->bind(0);
    mainBuffer->depthTexture->bind(1);
    waterBuffer->colorTexture->bind(2);
    waterBuffer->depthTexture->bind(3);
    skyBuffer->colorTexture->bind(4);

    auto shader = AssetManager::Instance()->getShader("combine");
    shader->bind();
    shader->setInt("uMainColor", 0);
    shader->setInt("uMainDepth", 1);
    shader->setInt("uWaterColor", 2);
    shader->setInt("uWaterDepth", 3);
    shader->setInt("uSkyColor", 4);
    shader->setFloat("uCameraNear", camera->near_);
    shader->setFloat("uCameraFar", camera->far_);

    fullscreenQuad->draw();
  }

  { // UI
    glDisable(GL_DEPTH_TEST);

    auto shader = AssetManager::Instance()->getShader("lines");
    shader->bind();
    shader->setMat4("uProjView", glm::ortho(-width/2.0f, width/2.0f, -height/2.0f, height/2.0f));
    shader->setMat4("uModel", glm::mat4(1.0f));
    crosshair->draw();

    renderTime = (double)(SDL_GetPerformanceCounter() - renderStart) / SDL_GetPerformanceFrequency();

    TickMeasure& tickMeasure = ::tickMeasure;
    std::string fpsStr = "FPS:" + std::to_string(tickMeasure.ticks.size());
    // std::string updateStr = "Update:" + std::to_string(int(updateTime * 1000.0)) + "ms";
    // std::string renderStr = "Render:" + std::to_string(int(renderTime * 1000.0)) + "ms";

    auto font = AssetManager::Instance()->getFont("exqt-ascii-mono.ttf");
    font->draw(ctx, fpsStr, 0, height - 24);

    glEnable(GL_DEPTH_TEST);
  }

}

void Game::setGameSize(int width, int height) {
  this->width = width;
  this->height = height;
  camera->ratio = (float)width / (float)height;

  mainBuffer = std::make_shared<Framebuffer>(width, height);
  waterBuffer = std::make_shared<Framebuffer>(width, height);
  skyBuffer = std::make_shared<Framebuffer>(width, height);
}

void Game::onRPC(DataReadStream& stream) {
  std::string name = stream.popString();

  if (name == "HELLO") {
    userId = stream.pop<int>();
  }
  else if (name == "OBJECT_SYNC") {
    auto data = stream.popVector<std::byte>();
    auto networkObject = NetworkObjectData::fromByteArray(data);
    auto id = networkObject.id;

    if (objects.count(id) == 1) {
      if (networkObject.owner != userId) {
        Object* object = objects[id];
        auto stream = DataReadStream(networkObject.data);
        object->deserialize(stream);
      }
    }
    else {
      auto fn = objectCreators[networkObject.type];
      if (fn) fn(networkObject);
      else std::cerr << "ERROR: unknown object type: " << networkObject.type << std::endl;
    }

  } else {
    std::cerr << "ERROR: unknown RPC response: " << name << std::endl;
    std::exit(1);
  }
}

void Game::syncOwnedObjects() {
  auto& networkManager = NetworkManager::Instance();

  for (auto& [id, object] : ownedObjects) {
    auto stream = DataWriteStream();
    object->serialize(stream);
    auto data = stream.data;

    auto networkObject = NetworkObjectData();
    networkObject.id = id;
    networkObject.owner = userId;
    networkObject.type = object->getNetworkType();
    networkObject.data = data;

    auto objData = networkObject.toByteArray();
    networkManager.callRPC("OBJECT_SYNC", objData);
  }
}

void Game::processChunks() {
  // world->update(player->position);
  // worldLoader->requestAroundPlayer(player->position.x, player->position.y, player->position.z);

  const int maxBuildsPerFrame = 4;
  for (int i = 0; i < maxBuildsPerFrame && !meshBuildQueue.isEmpty(); i++) {
    auto id = meshBuildQueue.pop();
    auto [ox, oy, oz] = id;
    auto data = world->getChunkData(ox, oy, oz);
    if (data == nullptr) continue;
    auto chunkKey = world->toChunkKey(ox, oy, oz);

    if (chunkObjects.count(chunkKey) == 0) {
      std::cerr << "ERROR: chunk not found: " << ox << ", " << oy << ", " << oz << std::endl;
      std::exit(1);
    }

    auto chunkObject = chunkObjects[chunkKey];
    chunkObject->setChunkData(data);
    chunkObject->buildMesh();
  }

  /*
  auto& unloadQueue = world->getUnloadQueue();
  while (!unloadQueue.empty()) {
    auto id = unloadQueue.front(); unloadQueue.pop();
    auto [ox, oy, oz] = id;
    auto chunkKey = world->toChunkKey(ox, oy, oz);

    if (chunkObjects.find(chunkKey) != chunkObjects.end()) {
      auto chunkObject = chunkObjects[chunkKey];
      auto it = std::find(objects.begin(), objects.end(), chunkObject);
      if (it != objects.end()) {
        objects.erase(it);
      }
      chunkObjects.erase(chunkKey);
      delete chunkObject;
    }
  }
  */
}

void Game::registerObjects() {
  objectCreators["PLAYER"] = [&](NetworkObjectData data) {
    auto stream = DataReadStream(data.data);
    if (userId == data.owner) {
      player = new MyPlayer();
      player->deserialize(stream);
      player->setCollisionMap(collisionMap);
      player->setCamera(camera);
      player->setWorld(world);
      objects[data.id] = player;
      ownedObjects[data.id] = player;
    }
    else {
      auto otherPlayer = new OtherPlayer();
      otherPlayer->deserialize(stream);
      objects[data.id] = otherPlayer;
    }
  };

  objectCreators["CHUNK"] = [&](NetworkObjectData data) {
    auto stream = DataReadStream(data.data);
    auto ox = stream.pop<int>();
    auto oy = stream.pop<int>();
    auto oz = stream.pop<int>();
    auto chunkData = std::make_shared<ChunkData>();
    chunkData->copyFromByteArray(stream.popVector<std::byte>());

    auto chunkObject = new ChunkObject(world, ox, oy, oz);
    world->setChunkData(ox, oy, oz, chunkData);
    chunkObject->setChunkData(chunkData);
    chunkObjects[world->toChunkKey(ox, oy, oz)] = chunkObject;
    objects[data.id] = chunkObject;

    meshBuildQueue.updateChunk(ox, oy, oz);
  };
}
