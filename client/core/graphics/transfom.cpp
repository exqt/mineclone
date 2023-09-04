#include "transform.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Transform::Transform() {
  mat = glm::mat4(1.0f);
}

Transform::~Transform() {

}

void Transform::scale(glm::vec2 scale) {
  mat = glm::scale(mat, glm::vec3(scale, 1));
}

void Transform::rotate(float rotation) {
  mat = glm::rotate(mat, rotation, glm::vec3(0, 0, 1));
}

void Transform::translate(glm::vec2 translation) {
  mat = glm::translate(mat, glm::vec3(translation, 0));
}