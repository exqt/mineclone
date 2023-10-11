#include "drawCommand.hpp"

void DrawCommand::draw() {
  shader->bind();

  for (const auto& [name, value] : shaderParams) {
    std::visit([&](auto&& arg) {
      using T = std::decay_t<decltype(arg)>;
      if constexpr (std::is_same_v<T, float>) {
        shader->setFloat(name, arg);
      } else if constexpr (std::is_same_v<T, glm::vec2>) {
        shader->setVec2(name, arg);
      } else if constexpr (std::is_same_v<T, glm::vec3>) {
        shader->setVec3(name, arg);
      } else if constexpr (std::is_same_v<T, glm::vec4>) {
        shader->setVec4(name, arg);
      } else if constexpr (std::is_same_v<T, glm::mat4>) {
        shader->setMat4(name, arg);
      }
    }, value);
  }

  mesh->draw();
}
