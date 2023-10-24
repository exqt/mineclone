#pragma once

#include <glm/glm.hpp>

#include "../../core/object.hpp"
#include "../collision.hpp"
#include "../../core/graphics/camera.hpp"
#include "../../core/graphics/mesh.hpp"
#include "../../core/graphics/lines.hpp"
#include "../inventory.hpp"

class PlayerBase : public Object {
public:
  glm::vec3 position;

  void serialize(DataWriteStream& stream) override;
  void deserialize(DataReadStream& stream) override;

  virtual std::string getNetworkType() { return "PLAYER"; }
};


class MyPlayer : public PlayerBase {
public:
  MyPlayer();
  ~MyPlayer();

  Camera* camera;
  World* world;
  Inventory* inventory;

  Lines* previewBox;

  void setWorld(World* world);
  void setPosition(glm::vec3 nwPos);
  void setCamera(Camera* camera);
  void setCollisionMap(CollisionMapPtr collisionMap);

  void update(float dt) override;
  void draw(DrawContext& ctx) override;

private:
  bool isGrounded = false;
  bool isFlying = false;

  AABBColliderPtr collider;
  CollisionMapPtr collisionMap;

  float jumpBufferCounter = 0.0;

  void handleMovement(float dt);
  void handleBlockAction(float dt);
  void placeBlock(int x, int y, int z, BlockType block);

  std::vector<CollisionInfo> move(glm::vec3 dir);

  glm::vec3 velocity;

  bool isTargetingBlock = false;
  int targetBlockX = 0, targetBlockY = 0, targetBlockZ = 0;
  int facingBlockX = 0, facingBlockY = 0, facingBlockZ = 0;

  float sizeX = 0.75f, sizeY = 1.75f, sizeZ = 0.75f;
  glm::vec3 colliderOffset = glm::vec3(-sizeX/2, 0.0f, -sizeX/2);
  glm::vec3 cameraOffset = glm::vec3(0.0f, sizeY - 0.1, 0.0f);
};

class OtherPlayer : public PlayerBase {
public:
  OtherPlayer();
  ~OtherPlayer();

  void update(float dt) override;
  void draw(DrawContext& ctx) override;

private:
  MeshPtr mesh;
};
