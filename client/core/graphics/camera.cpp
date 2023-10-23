#include "camera.hpp"

// https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/camera.h

Camera::Camera() {
  position = {0, 0, 0};
  worldUp = {0, 1, 0};

  updateCameraVectors();
}

Camera::~Camera()
{
}

void Camera::move(float deltaFront, float deltaRight) {
  position += front * deltaFront;
  position += right * deltaRight;
}

void Camera::handleMouseInput(float deltaYaw, float deltaPitch) {
  yaw += deltaYaw;
  pitch += deltaPitch;

  pitch = glm::clamp(pitch, -89.0f, 89.0f);
  yaw = glm::mod(yaw, 360.0f);

  updateCameraVectors();
}

void Camera::updateCameraVectors() {
  front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  front.y = sin(glm::radians(pitch));
  front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  front = glm::normalize(front);
  right = glm::normalize(glm::cross(front, worldUp));
  up = glm::normalize(glm::cross(right, front));
}

void Camera::lookAt(glm::vec3 target) {
  front = glm::normalize(target - position);
  right = glm::normalize(glm::cross(front, worldUp));
  up = glm::normalize(glm::cross(right, front));
}

// OrthographicsCamera
OrthographicCamera::OrthographicCamera(float width, float height) {
  this->width = width;
  this->height = height;
}

glm::mat4 OrthographicCamera::getProjectionViewMatrix() {
  glm::mat4 matView = glm::lookAt(position, position + front, up);
  glm::mat4 matProjection = glm::ortho(-width/2, width/2, -height/2, height/2, near_, far_);
  return matProjection * matView;
}

// PerspectiveCamera
PerspectiveCamera::PerspectiveCamera(float ratio) {
  this->ratio = ratio;
}

glm::mat4 PerspectiveCamera::getProjectionViewMatrix() {
  glm::mat4 matView = glm::lookAt(position, position + front, up);
  glm::mat4 matProjection = glm::perspective(glm::radians(fov), ratio, near_, far_);
  return matProjection * matView;
}
