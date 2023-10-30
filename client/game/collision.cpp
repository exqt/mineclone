#include "collision.hpp"

#include <cmath>
#include <iostream>
#include <algorithm>
#include <vector>
#include <utility>
#include <glm/gtx/norm.hpp>

const float EPS = 1e-6;

#pragma region AABB

AABB::AABB() { }

AABB::AABB(glm::vec3 pos, float w, float h, float d) {
  this->pos = pos;
  this->w = w;
  this->h = h;
  this->d = d;
}

bool AABB::intersects(const AABB& other) {
  float px = std::max(pos.x, other.pos.x);
  float py = std::max(pos.y, other.pos.y);
  float pz = std::max(pos.z, other.pos.z);
  float qx = std::min(pos.x + w, other.pos.x + other.w);
  float qy = std::min(pos.y + h, other.pos.y + other.h);
  float qz = std::min(pos.z + d, other.pos.z + other.d);

  return px < qx && py < qy && pz < qz;
}

bool AABB::contains(glm::vec3 point) {
  return (
    pos.x < point.x && point.x < pos.x + w &&
    pos.y < point.y && point.y < pos.y + h &&
    pos.z < point.z && point.z < pos.z + d);
}

AABB AABB::minkowskiSum(const AABB& other) const {
  return AABB(
    pos + other.pos,
    w + other.w,
    h + other.h,
    d + other.d
  );
}

std::vector<std::pair<float, glm::vec3>> AABB::raycast(glm::vec3 origin, glm::vec3 dir) {
  std::vector<std::pair<float, glm::vec3>> tv;

  float tx1 = nan(""), tx2 = nan(""), ty1 = nan(""), ty2 = nan(""), tz1 = nan(""), tz2 = nan("");

  if (dir.x != 0.0) {
    tx1 = (pos.x - origin.x) / dir.x;
    tx2 = (pos.x + w - origin.x) / dir.x;
  }
  else {
    if (pos.x < origin.x && origin.x < pos.x + w) {
      tx1 = -INFINITY;
      tx2 = INFINITY;
    } else return {};
  }

  if (dir.y != 0.0) {
    ty1 = (pos.y - origin.y) / dir.y;
    ty2 = (pos.y + h - origin.y) / dir.y;
  }
  else {
    if (pos.y < origin.y && origin.y < pos.y + h) {
      ty1 = -INFINITY;
      ty2 = INFINITY;
    } else return {};
  }

  if (dir.z != 0.0) {
    tz1 = (pos.z - origin.z) / dir.z;
    tz2 = (pos.z + d - origin.z) / dir.z;
  }
  else {
    if (pos.z < origin.z && origin.z < pos.z + d) {
      tz1 = -INFINITY;
      tz2 = INFINITY;
    } else return {};
  }

  float tStart = std::max(std::max(std::min(tx1, tx2), std::min(ty1, ty2)), std::min(tz1, tz2));
  float tEnd = std::min(std::min(std::max(tx1, tx2), std::max(ty1, ty2)), std::max(tz1, tz2));

  if (tEnd < 0) return tv;
  if (tStart > tEnd) return tv;

  glm::vec3 tStartNormal, tEndNormal;
  if (tStart == tx1) tStartNormal = glm::vec3(-1, 0, 0);
  else if (tStart == tx2) tStartNormal = glm::vec3(1, 0, 0);
  else if (tStart == ty1) tStartNormal = glm::vec3(0, -1, 0);
  else if (tStart == ty2) tStartNormal = glm::vec3(0, 1, 0);
  else if (tStart == tz1) tStartNormal = glm::vec3(0, 0, -1);
  else if (tStart == tz2) tStartNormal = glm::vec3(0, 0, 1);

  if (tEnd == tx1) tEndNormal = glm::vec3(-1, 0, 0);
  else if (tEnd == tx2) tEndNormal = glm::vec3(1, 0, 0);
  else if (tEnd == ty1) tEndNormal = glm::vec3(0, -1, 0);
  else if (tEnd == ty2) tEndNormal = glm::vec3(0, 1, 0);
  else if (tEnd == tz1) tEndNormal = glm::vec3(0, 0, -1);
  else if (tEnd == tz2) tEndNormal = glm::vec3(0, 0, 1);

  if (tStart < 0) {
    tv.emplace_back(tEnd, tEndNormal);
  }
  else {
    tv.emplace_back(tStart, tStartNormal);
    tv.emplace_back(tEnd, tEndNormal);
  }

  return tv;
}

AABB minkowskiSum(AABB& a, AABB& b) {
  return AABB(
    a.pos + b.pos,
    a.w + b.w,
    a.h + b.h,
    a.d + b.d
  );
}

float nearest(float x, float y, float p) {
  if (std::abs(x - p) < std::abs(y - p)) return x;
  return y;
}

glm::vec3 AABB::getNearestCorner(glm::vec3 pts) {
  return glm::vec3(
    nearest(pos.x, pos.x + w, pts.x),
    nearest(pos.y, pos.y + h, pts.y),
    nearest(pos.z, pos.z + d, pts.z)
  );
}

std::optional<CollisionInfo> AABB::collide(const AABB& other, glm::vec3 dir) {
  AABB negated = AABB({-(pos.x + w), -(pos.y + h), -(pos.z + d)}, w, h, d);
  AABB minkowksiDiff = other.minkowskiSum(negated);

  bool overlaps = false;
  float ti = nan("");
  glm::vec3 normal, touch;

  if (intersects(other)) {
    overlaps = true;
  }
  else {
    auto tv = minkowksiDiff.raycast(glm::vec3(0, 0, 0), dir);

    // item tunnels into other
    if (
      tv.size() >= 2 &&
      tv[0].first < 1 &&
      std::abs(tv[0].first - tv[1].first) > EPS &&
      ((0 < tv[0].first + EPS) || (tv.size() > 1 && 0 == tv[0].first && tv[1].first > 0))
    ) {
      ti = tv[0].first;
      normal = tv[0].second;
      overlaps = false;
    }
  }

  if (std::isnan(ti)) return std::nullopt;

  // TODO: handle overlaps case
  if (overlaps) {
    if (dir == glm::vec3(0, 0, 0)) {
    }
    else {
    }
  }
  else {
    touch = pos + dir * ti;
  }

  if (normal.x == 1) touch.x = other.pos.x + other.w;
  else if (normal.x == -1) touch.x = other.pos.x - w;
  else if (normal.y == 1) touch.y = other.pos.y + other.h;
  else if (normal.y == -1) touch.y = other.pos.y - h;
  else if (normal.z == 1) touch.z = other.pos.z + other.d;
  else if (normal.z == -1) touch.z = other.pos.z - d;

  return CollisionInfo{
    .t = ti,
    .normal = normal,
    .touch = touch,
    .inside = false
  };
}

#pragma endregion AABB

#pragma region CollisionMap

CollisionMap::CollisionMap(World* world) {
  this->world = world;
}

CollisionMap::~CollisionMap() {
}

std::tuple<int, int, int> getKey(float x, float y, float z) {
  return std::make_tuple(
    std::floor(x/HASH_SPACE_SIZE),
    std::floor(y/HASH_SPACE_SIZE),
    std::floor(z/HASH_SPACE_SIZE)
  );
}

AABBColliderPtr CollisionMap::create(glm::vec3 pos, float w, float h, float d, ColliderTag tag) {
  AABBColliderPtr collider = std::make_shared<AABBCollider>(shared_from_this(), pos, w, h, d, tag);
  return collider;
}

void CollisionMap::addToCells(AABBColliderPtr collider) {
  float px = collider->aabb.pos.x;
  float py = collider->aabb.pos.y;
  float pz = collider->aabb.pos.z;
  float qx = collider->aabb.pos.x + collider->aabb.w;
  float qy = collider->aabb.pos.y + collider->aabb.h;
  float qz = collider->aabb.pos.z + collider->aabb.d;

  auto cells = getCells(px, py, pz, qx, qy, qz);
  for (auto cell : cells) {
    spatialMap[cell].insert(collider);
  }
}

void CollisionMap::removeFromCells(AABBColliderPtr collider) {
  float px = collider->aabb.pos.x;
  float py = collider->aabb.pos.y;
  float pz = collider->aabb.pos.z;
  float qx = collider->aabb.pos.x + collider->aabb.w;
  float qy = collider->aabb.pos.y + collider->aabb.h;
  float qz = collider->aabb.pos.z + collider->aabb.d;

  auto cells = getCells(px, py, pz, qx, qy, qz);
  for (auto cell : cells) {
    spatialMap[cell].erase(collider);
  }
}

void CollisionMap::updateCells(AABBColliderPtr collider, glm::vec3 newPos, float w, float h, float d) {
  removeFromCells(collider);
  collider->aabb.pos = newPos;
  collider->aabb.w = w;
  collider->aabb.h = h;
  collider->aabb.d = d;
  addToCells(collider);
}

std::vector<CellKey> CollisionMap::getCells(float px, float py, float pz, float qx, float qy, float qz) {
  int ipx = std::floor(px / HASH_SPACE_SIZE);
  int ipy = std::floor(py / HASH_SPACE_SIZE);
  int ipz = std::floor(pz / HASH_SPACE_SIZE);
  int iqx = std::floor(qx / HASH_SPACE_SIZE);
  int iqy = std::floor(qy / HASH_SPACE_SIZE);
  int iqz = std::floor(qz / HASH_SPACE_SIZE);

  std::vector<CellKey> cells;
  for (int i = ipx; i <= iqx; i++) {
    for (int j = ipy; j <= iqy; j++) {
      for (int k = ipz; k <= iqz; k++) {
        cells.push_back(std::make_tuple(i, j, k));
      }
    }
  }

  return cells;
}

std::vector<AABBColliderPtr> CollisionMap::getColliders(float px, float py, float pz, float qx, float qy, float qz, bool includeBlocks) {
  auto cells = getCells(px, py, pz, qx, qy, qz);

  std::vector<AABBColliderPtr> colliders;
  for (auto cell : cells) {
    for (auto collider : spatialMap[cell]) {
      colliders.push_back(collider);
    }
  }

  if (world != nullptr && includeBlocks) {
    int ipx = std::floor(px);
    int ipy = std::floor(py);
    int ipz = std::floor(pz);
    int iqx = std::floor(qx);
    int iqy = std::floor(qy);
    int iqz = std::floor(qz);

    for (int i = ipx; i <= iqx; i++) {
      for (int j = ipy; j <= iqy; j++) {
        for (int k = ipz; k <= iqz; k++) {
          auto block = world->getBlock(i, j, k);
          if (!IS_SOLID(block)) continue;

          auto collider = std::make_shared<AABBCollider>(
            enable_shared_from_this::shared_from_this(),
            glm::vec3{i, j, k},
            1, 1, 1,
            ColliderTag::SOLID_BLOCK
          );

          colliders.push_back(collider);
        }
      }
    }
  }

  return colliders;
}

std::vector<CollisionInfo> CollisionMap::project(AABB aabb, glm::vec3 goal) {
  // bounding box
  float px = std::min(aabb.pos.x         , goal.x);
  float py = std::min(aabb.pos.y         , goal.y);
  float pz = std::min(aabb.pos.z         , goal.z);
  float qx = std::max(aabb.pos.x + aabb.w, goal.x + aabb.w);
  float qy = std::max(aabb.pos.y + aabb.h, goal.y + aabb.h);
  float qz = std::max(aabb.pos.z + aabb.d, goal.z + aabb.d);

  std::vector<CollisionInfo> collisions;
  for (auto other : getColliders(px, py, pz, qx, qy, qz)) {
    glm::vec3 dir = goal - aabb.pos;
    auto _col = aabb.collide(other->aabb, dir);
    if (!_col) continue;

    CollisionInfo col = _col.value();
    col.collider = other;
    col.move = dir;

    collisions.push_back(col);
  }

  std::sort(collisions.begin(), collisions.end(), [&](CollisionInfo& a, CollisionInfo& b) {
    if (a.t == b.t) {
      // square distance to aabb
      glm::vec3 aPos = a.collider->aabb.pos;
      glm::vec3 bPos = b.collider->aabb.pos;
      return glm::distance2(aabb.pos, aPos) < glm::distance2(aabb.pos, bPos);
    }
    return a.t < b.t;
  });

  return collisions;
}

std::vector<CollisionInfo> CollisionMap::raycast(glm::vec3 origin, glm::vec3 dir) {
  glm::vec3 goal = origin + dir;
  // bounding box
  float px = std::min(origin.x, goal.x);
  float py = std::min(origin.y, goal.y);
  float pz = std::min(origin.z, goal.z);
  float qx = std::max(origin.x, goal.x);
  float qy = std::max(origin.y, goal.y);
  float qz = std::max(origin.z, goal.z);

  std::vector<CollisionInfo> collisions;

  for (auto collider : getColliders(px, py, pz, qx, qy, qz)) {
    auto result = collider->aabb.raycast(origin, dir);
    if (result.size() == 0) continue;
    auto [t, normal] = result[0];

    collisions.push_back(CollisionInfo{
      .t = t,
      .normal = normal,
      .touch = origin + dir * t,
      .collider = collider
    });
  }

  std::sort(collisions.begin(), collisions.end(), [&](CollisionInfo& a, CollisionInfo& b) {
    return a.t < b.t;
  });

  return collisions;
}

std::vector<AABBColliderPtr> CollisionMap::queryAABB(float px, float py, float pz, float qx, float qy, float qz, bool includeBlocks) {
  auto colliders = getColliders(px, py, pz, qx, qy, qz, includeBlocks);

  // filter
  colliders.erase(
    std::remove_if(colliders.begin(), colliders.end(), [&](AABBColliderPtr collider) {
      return !collider->aabb.intersects(AABB(glm::vec3{px, py, pz}, qx - px, qy - py, qz - pz));
    }),
    colliders.end()
  );

  return colliders;
}

#pragma endregion CollisionMap

#pragma region AABBCollider

AABBCollider::AABBCollider(CollisionMapPtr collisionMap, glm::vec3 pos, float w, float h, float d, ColliderTag tag) {
  this->collisionMap = collisionMap;
  this->aabb = AABB(pos, w, h, d);
  this->tag = tag;
}

AABBCollider::~AABBCollider() {
}

void AABBCollider::update(glm::vec3 pos, float w, float h, float d) {
  collisionMap->updateCells(shared_from_this(), pos, w, h, d);
}

std::tuple<glm::vec3, std::vector<CollisionInfo>> AABBCollider::check(glm::vec3 goal) {
  std::vector<CollisionInfo> collisions;

  collisionMap->removeFromCells(shared_from_this());
  auto projectedCols = collisionMap->project(this->aabb, goal);

  int maxCollisionCount = 3;
  while (projectedCols.size() > 0 && maxCollisionCount-- > 0) {
    auto col = projectedCols[0];
    if (col.inside) break;

    collisions.push_back(col);

    auto [nextGoal, cols] = slide(col, aabb, goal);
    projectedCols = cols;
    goal = nextGoal;
  }

  collisionMap->addToCells(shared_from_this());
  return std::make_tuple(goal, collisions);
}

std::tuple<glm::vec3, std::vector<CollisionInfo>> AABBCollider::move(glm::vec3 goal) {
  auto [actual, collisions] = check(goal);
  collisionMap->updateCells(shared_from_this(), actual, aabb.w, aabb.h, aabb.d);
  return std::make_tuple(actual, collisions);
}

std::tuple<glm::vec3, std::vector<CollisionInfo>> AABBCollider::slide(CollisionInfo& info, AABB aabb, glm::vec3 goal) {
  if (info.move != glm::vec3(0, 0, 0)) {
    if (info.normal.x != 0) goal.x = info.touch.x;
    else if (info.normal.y != 0) goal.y = info.touch.y;
    else if (info.normal.z != 0) goal.z = info.touch.z;
  }

  auto collisions = this->collisionMap->project(aabb, goal);

  return std::make_tuple(goal, collisions);
}

#pragma endregion AABBCollider
