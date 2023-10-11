#pragma once

#include <functional>

#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi

class Camera {
public:
  glm::vec3 position, front, up, right, worldUp;

  float yaw = 0, pitch = 0, roll = 0;
  float fov = 45;
  float near_ = 0.1f;
  float far_ = 100.0f;

  Camera();
  ~Camera();

  void move(float deltaFront, float deltaRight);
  void handleMouseInput(float deltaYaw, float deltaPitch);
  void updateCameraVectors();
  void lookAt(glm::vec3 target);

  virtual glm::mat4 getProjectionViewMatrix() = 0;

private:
};

class OrthographicCamera : public Camera {
public:
  OrthographicCamera(float width, float height);

  float width, height;

  glm::mat4 getProjectionViewMatrix() override;
};

class PerspectiveCamera : public Camera {
public:
  PerspectiveCamera(float ratio);

  float ratio = 800.0f/600.0f;

  glm::mat4 getProjectionViewMatrix() override;
};
