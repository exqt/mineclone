#pragma once

#include <glm/glm.hpp>

class Transform {
public:
  Transform();
  ~Transform();

  void translate(glm::vec2 translation);
  void scale(glm::vec2 scale);
  void rotate(float rotation);

  glm::mat4 getMatrix() { return mat; }

private:
  // glm::vec3 position;

  glm::mat4 mat;
};
