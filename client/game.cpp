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
  camera->far = 1000.0;


  player = new Player(glm::vec3(5.0, 50.0, 5.0));
  player->setCollisionMap(collisionMap);
  player->setCamera(camera);
  player->setWorld(world);

  crosshair = new Lines();
  crosshair->update({
    4, 0, 0,  -4, 0, 0,
    0, 4, 0,  0, -5, 0,
  }, {
    1, 1, 1,  1, 1, 1,
    1, 1, 1,  1, 1, 1,
  });

  skyDome = new SkyDome();

  objects.push_back(player);

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

  for (auto& object : objects) {
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

  skyDome->setTime(time);
  skyDome->draw(ctx);

  for (auto& [key, chunkObject] : chunkObjects) {
    chunkObject->draw(ctx);
  }

  for (auto& object : objects) {
    object->draw(ctx);
  }

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
  // font->draw(&ctx, updateStr, 0, height - 48);
  // font->draw(&ctx, renderStr, 0, height - 72);
}

void Game::setGameSize(int width, int height) {
  this->width = width;
  this->height = height;
  camera->ratio = (float)width / (float)height;
}

void Game::processChunks() {
  world->update(player->position);

  const int maxBuildsPerFrame = 16;
  auto &meshBuildQueue = world->getRenderQueue();
  for (int i = 0; i < maxBuildsPerFrame && !meshBuildQueue.empty(); i++) {
    auto id = meshBuildQueue.front(); meshBuildQueue.pop();
    auto data = world->getChunkData(id);
    if (data == nullptr) continue;
    auto [ox, oy, oz] = id;
    auto chunkKey = world->toChunkKey(ox, oy, oz);

    ChunkObject* chunkObject = nullptr;
    if (chunkObjects.find(chunkKey) == chunkObjects.end()) {
      chunkObject = new ChunkObject(world, ox, oy, oz);
      chunkObjects[chunkKey] = chunkObject;
    }
    else {
      chunkObject = chunkObjects[chunkKey];
    }

    chunkObject->setChunkData(data);
    chunkObject->buildMesh();
  }

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
}
