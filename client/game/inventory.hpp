#pragma once

#include "../../common/block.hpp"
#include "../core/graphics/drawContext.hpp"
#include "../core/graphics/mesh.hpp"
#include <array>

class Inventory {
public:
  Inventory();
  ~Inventory();

  BlockType getCurrentBlock();
  void update(float dt);
  void draw(DrawContext& ctx);

private:
  int currentIdx = 0;
  std::array<BlockType, 16> blocks;
  MeshPtr mesh, selectionQuadMesh;
};
