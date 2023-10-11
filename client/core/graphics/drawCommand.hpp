#pragma once

#include <glm/glm.hpp>
#include "mesh.hpp"
#include "shader.hpp"
#include <map>
#include <variant>

struct DrawCommand {
  MeshPtr mesh;
  ShaderPtr shader;
  std::map<std::string, std::variant<float, glm::vec2, glm::vec3, glm::vec4, glm::mat4>> shaderParams;

  void draw();
};
