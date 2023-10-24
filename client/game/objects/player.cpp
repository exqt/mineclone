#include "player.hpp"

#include "../../config.hpp"
#include "../../core/input.hpp"
#include "../gameState.hpp"
#include "../../core/managers/assetManager.hpp"
#include "../../core/managers/networkManager.hpp"
#include "../../../common/byteStream.hpp"
#include "../meshData.hpp"

void PlayerBase::serialize(DataWriteStream& stream) {
  stream.push(position.x);
  stream.push(position.y);
  stream.push(position.z);
}

void PlayerBase::deserialize(DataReadStream& stream) {
  position.x = stream.pop<float>();
  position.y = stream.pop<float>();
  position.z = stream.pop<float>();
}

#pragma region MyPlayer

MyPlayer::MyPlayer() {
  position = glm::vec3(0.0);
  velocity = glm::vec3(0.0);

  previewBox = new Lines();

  auto boxVerts = BLOCK_PREVIEW_LINES_VERTICES;
  for (auto &v : boxVerts) {
    if (v == 0) v = -0.01;
    else if (v == 1) v = 1.01;
  }
  previewBox->update(boxVerts, BLOCK_PREVIEW_LINES_COLORS);
}

MyPlayer::~MyPlayer() {
}

#include "../../imgui/infoView.hpp"

// TODO: control logic
void MyPlayer::update(float dt) {
  GameState& state = GameState::Instance();
  Config config;

  if (state.isMenuOpen()) return;

  handleBlockAction(dt);
  handleMovement(dt);

  DebugInfo& debugInfo = DebugInfo::get();
  debugInfo.playerPos = position;

  jumpBufferCounter -= dt;
}

void MyPlayer::draw(DrawContext& ctx) {
  if (isTargetingBlock) {
    auto shader = AssetManager::Instance()->getShader("lines");
    shader->bind();
    shader->setMat4("uProjView", ctx.camera->getProjectionViewMatrix());
    shader->setMat4("uModel", glm::translate(glm::mat4(1.0), {targetBlockX, targetBlockY, targetBlockZ}));
    previewBox->draw();
  }
}

void MyPlayer::handleMovement(float dt) {
  Input& input = Input::Instance();
  Config config;

  if (input.isKeyPressed(SDL_SCANCODE_1)) isFlying = !isFlying;

  // mouse handle
  const float mouseIntensity = 1.0;
  auto mouseMove = input.getMouseMove();
  camera->handleMouseInput(mouseMove.x*mouseIntensity/10.0, -mouseMove.y*mouseIntensity/10.0);

  float SPEED = 10.0;
  if (input.isKeyDown(config.sprint)) SPEED *= 2.0;

  if (!isFlying) {
    if (input.isKeyPressed(config.jump)) {
      jumpBufferCounter = 0.1;
    }

    float theta = glm::radians(camera->yaw);
    int inputX = (input.isKeyDown(config.forward) ? 1 : 0) + (input.isKeyDown(config.backward) ? -1 : 0);
    int inputZ = (input.isKeyDown(config.right) ? 1 : 0) + (input.isKeyDown(config.left) ? -1 : 0);
    float inputL = std::hypot(inputX, inputZ);

    float fvX = cos(theta);
    float fvZ = sin(theta);
    auto xzmove = (glm::vec3(fvX, 0, fvZ)*(float)inputX + glm::vec3(-fvZ, 0, fvX)*(float)inputZ);
    if (inputL != 0.0) xzmove *= SPEED / sqrt(2.0);

    velocity.x = xzmove.x;
    velocity.z = xzmove.z;
    velocity.y += -20*dt;

    if (jumpBufferCounter > 0.0 && isGrounded) {
      velocity.y = 7.0;
      jumpBufferCounter = 0.0;
    }

    // velocity.y = 0;

    auto collisions = move(velocity * dt);

    isGrounded = false;
    for (auto& collision : collisions) {
      if (collision.normal.y == 1.0) {
        velocity.y = 0;
        isGrounded = true;
      }
      else if (collision.normal.y == -1.0) {
        velocity.y = 0;
      }
    }
  }
  else {
    float deltaRight = 0, deltaFront = 0;
    if (input.isKeyDown(config.right)) deltaRight += SPEED;
    if (input.isKeyDown(config.left)) deltaRight -= SPEED;
    if (input.isKeyDown(config.forward)) deltaFront += SPEED;
    if (input.isKeyDown(config.backward)) deltaFront -= SPEED;

    glm::vec3 dir = camera->front * deltaFront + camera->right * deltaRight;
    if (input.isKeyDown(config.jump)) dir.y += SPEED;

    move(dir * dt);
  }
}

#include <iostream>

void MyPlayer::handleBlockAction(float dt) {
  Input& input = Input::Instance();
  GameState& state = GameState::Instance();
  Config config;

  if (state.isMenuOpen()) return;

  auto cols = collisionMap->raycast(camera->position, camera->front * 4.5f);
  auto blockCol = std::find_if(cols.begin(), cols.end(), [](CollisionInfo& col) {
    return col.collider->tag == ColliderTag::SOLID_BLOCK;
  });

  if (blockCol != cols.end()) {
    glm::vec3 targetPos = blockCol->touch - blockCol->normal * 0.5f;
    glm::vec3 facingPos = blockCol->touch + blockCol->normal * 0.5f;
    isTargetingBlock = true;
    targetBlockX = std::floor(targetPos.x);
    targetBlockY = std::floor(targetPos.y);
    targetBlockZ = std::floor(targetPos.z);
    facingBlockX = std::floor(facingPos.x);
    facingBlockY = std::floor(facingPos.y);
    facingBlockZ = std::floor(facingPos.z);
  }
  else {
    isTargetingBlock = false;
  }

  if (!isTargetingBlock) return;

  if (input.isKeyPressed(config.placeBlock)) {
    auto colliders = collisionMap->queryAABB(
      facingBlockX, facingBlockY, facingBlockZ,
      facingBlockX + 1, facingBlockY + 1, facingBlockZ + 1,
      false
    );

    if (colliders.size() > 0) {
      std::cout << "can't place block here" << std::endl;
      return;
    }

    placeBlock(facingBlockX, facingBlockY, facingBlockZ, inventory->getCurrentBlock());
  }
  else if (input.isKeyPressed(config.breakBlock)) {
    placeBlock(targetBlockX, targetBlockY, targetBlockZ, BlockType::NONE);
  }
}

void MyPlayer::setPosition(glm::vec3 nwPos) {
  position = nwPos;
  camera->position = position + cameraOffset;
  if (collider) collider->update(position + colliderOffset, sizeX, sizeY, sizeZ);
}

void MyPlayer::setCollisionMap(CollisionMapPtr collisionMap) {
  this->collisionMap = collisionMap;
  this->collider = collisionMap->create(position + colliderOffset, sizeX, sizeY, sizeZ, ColliderTag::PLAYER);
}

void MyPlayer::setCamera(Camera* camera) {
  this->camera = camera;
  camera->position = position + cameraOffset;
}

void MyPlayer::setWorld(World* world) {
  this->world = world;
}

std::vector<CollisionInfo> MyPlayer::move(glm::vec3 dir) {
  auto [newPos, collisions] = collider->move(position + colliderOffset + dir);
  position = newPos - colliderOffset;
  camera->position = position + cameraOffset;
  return collisions;
}

void MyPlayer::placeBlock(int x, int y, int z, BlockType block) {
  auto& networkManager = NetworkManager::Instance();
  auto stream = DataWriteStream();
  stream.pushString("PLACE_BLOCK");
  stream.push(x);
  stream.push(y);
  stream.push(z);
  stream.push(block);
  networkManager.send(stream.data);
}

#pragma endregion

#pragma region OtherPlayer
#include "../meshData.hpp"

OtherPlayer::OtherPlayer() {
  position = glm::vec3(0.0);
  mesh = std::make_shared<Mesh>();

  // temporary player mesh
  auto playerMesh = BLOCK_VERTEX_POSITIONS;
  auto playerUVs = std::vector<float>(playerMesh.size());
  for (size_t i = 0; i < playerMesh.size(); i++) {
    if (i % 3 == 0) playerMesh[i] -= 0.5;
    else if (i % 3 == 1) playerMesh[i] *= 2;
    else if (i % 3 == 2) playerMesh[i] -= 0.5;
    playerUVs[i] = 1.0;
  }
  mesh->update(playerMesh, playerUVs);
}

OtherPlayer::~OtherPlayer() {
}

void OtherPlayer::update(float dt) {
}

void OtherPlayer::draw(DrawContext& ctx) {
  auto modelMat = glm::translate(glm::mat4(1.0), position);

  auto shader = AssetManager::Instance()->getShader("model");
  shader->bind();
  shader->setMat4("uModel", modelMat);
  shader->setMat4("uProjView", ctx.camera->getProjectionViewMatrix());

  mesh->draw();
}

#pragma endregion
