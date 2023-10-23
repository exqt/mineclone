#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "camera.hpp"
#include "frameBuffer.hpp"
#include "drawCommand.hpp"

struct DrawContext {
  DrawContext() {}

  Camera* camera;
  int width, height;

  FramebufferPtr mainBuffer;

  std::vector<DrawCommand> drawCommands;

  // bool isShadowPass = false;
  // Texture* shadowMap;
  // glm::mat4 shadowMatrix;
};
