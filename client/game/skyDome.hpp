#pragma once

#include "../core/graphics/mesh.hpp"
#include "../core/graphics/shader.hpp"
#include "../core/graphics/drawContext.hpp"
#include "../core/graphics/camera.hpp"

class SkyDome {
public:
  SkyDome();
  ~SkyDome();

  void setTime(float time);
  void draw(DrawContext& ctx);

private:
  Mesh* mesh;
  float time = 0.0f;
};
