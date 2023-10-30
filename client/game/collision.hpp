#pragma once

#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <tuple>
#include <optional>
#include <memory>

#include "../../common/world.hpp"

#include <glm/glm.hpp>

using CellKey = std::tuple<int, int, int>;
const int HASH_SPACE_SIZE = 1;

class CollisionMap;
class AABBCollider;

using CollisionMapPtr = std::shared_ptr<CollisionMap>;
using AABBColliderPtr = std::shared_ptr<AABBCollider>;

enum ColliderTag : unsigned char {
  SOLID_BLOCK,
  PLAYER,
  ENTITY,
  PROJECTILE
};

struct CollisionInfo {
  float t; // how far the AABB moved to cause the collision
  glm::vec3 normal; // normal of the surface collided with
  glm::vec3 touch; // position of AABB at time of the collision
  glm::vec3 move; // target - start
  bool inside; // whether the AABB was inside the other AABB at the start
  AABBColliderPtr collider;
};

struct AABB {
public:
  AABB();
  AABB(glm::vec3 pos, float w, float h, float d);

  glm::vec3 pos;
  float w, h, d;

  bool intersects(const AABB& other);
  bool contains(glm::vec3 point);

  AABB minkowskiSum(const AABB& other) const;
  std::optional<CollisionInfo> collide(const AABB& other, glm::vec3 dir);
  std::vector<std::pair<float, glm::vec3>> raycast(glm::vec3 origin, glm::vec3 dir);
  glm::vec3 getNearestCorner(glm::vec3 pts);

private:

  friend class AABBCollider;
};

// TODO: fix circular pointer, add
class CollisionMap : public std::enable_shared_from_this<CollisionMap> {
public:
  CollisionMap(World* world);
  ~CollisionMap();

  AABBColliderPtr create(glm::vec3 pos, float w, float h, float d, ColliderTag tag);
  std::vector<AABBColliderPtr> queryAABB(float px, float py, float pz, float qx, float qy, float qz, bool includeBlocks);
  // void destroy(AABBColliderPtr collider);

  std::vector<CollisionInfo> raycast(glm::vec3 origin, glm::vec3 dir);

private:
  World* world;
  std::map< std::tuple<int, int, int>, std::unordered_set<AABBColliderPtr> > spatialMap;

  void addToCells(AABBColliderPtr collider);
  void removeFromCells(AABBColliderPtr collider);
  void updateCells(AABBColliderPtr collider, glm::vec3 newPos, float w, float h, float d);

  std::vector<CollisionInfo> project(AABB aabb, glm::vec3 goal);

  std::vector<AABBColliderPtr> getColliders(float px, float py, float pz, float qx, float qy, float qz, bool includeBlocks = true);
  std::vector<CellKey> getCells(float px, float py, float pz, float qx, float qy, float qz);

  friend class AABBCollider;
};


class AABBCollider : public std::enable_shared_from_this<AABBCollider> {
public:
  AABBCollider(CollisionMapPtr collisionMap, glm::vec3 pos, float w, float h, float d, ColliderTag tag);
  ~AABBCollider();

  std::tuple<glm::vec3, std::vector<CollisionInfo>> check(glm::vec3 goal);
  std::tuple<glm::vec3, std::vector<CollisionInfo>> move(glm::vec3 goal);
  void update(glm::vec3 pos, float w, float h, float d);

  AABB aabb;
  ColliderTag tag;

private:
  CollisionMapPtr collisionMap;

  std::tuple<glm::vec3, std::vector<CollisionInfo>> slide(CollisionInfo& info, AABB aabb, glm::vec3 goal);

  friend class CollisionMap;
};
