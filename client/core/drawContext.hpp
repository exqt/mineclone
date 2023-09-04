#pragma once

#include <glm/glm.hpp>
#include "camera.hpp"

struct DrawContext {
  DrawContext() {}

  Camera* camera;
  int width, height;

  // bool isShadowPass = false;
  // Texture* shadowMap;
  // glm::mat4 shadowMatrix;
};
