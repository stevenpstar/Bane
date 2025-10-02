#include "glm/ext/matrix_clip_space.hpp"
#include "glm/trigonometric.hpp"
#include <bane/components/camera.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 position) {
  // Camera setup
  cameraTransform = glm::mat4(1.f);
  cameraPos = position;
  cameraTransform = glm::translate(cameraTransform, cameraPos);
  cameraTarget = glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z - 3.f);
  cameraDirection = glm::normalize(cameraPos - cameraTarget);
  up = glm::vec3(0.f, 1.f, 0.f);
  cameraRight = glm::normalize(glm::cross(up, cameraDirection));
  cameraUp = glm::cross(cameraDirection, cameraRight);
  projection = glm::perspective(glm::radians(60.f), 800.f / 600.f, -1.0f, 100.f);
}

glm::mat4 Camera::lookAtTarget(glm::vec3 target) {
  cameraTarget = target;
  cameraTransform = glm::lookAt(cameraPos, cameraTarget, cameraUp);
  glm::vec3 dir = cameraPos - cameraTarget;
  // setCameraDirection(dir);
  return cameraTransform;
}

glm::mat4 Camera::getTransform() const { return cameraTransform; }

void Camera::setPosition(glm::vec3 position) {
  cameraPos = position;
  cameraTransform = glm::translate(cameraTransform, cameraPos);
}

void Camera::setCameraDirection(glm::vec3 dir) {
  cameraDirection = dir;
  cameraRight = glm::normalize(glm::cross(up, cameraDirection));
  cameraUp = glm::cross(cameraDirection, cameraRight);
}

void Camera::setProjection(int width, int height, float fov) {
  projection = glm::perspective(glm::radians(fov), (float)width / (float)height, drawStart, drawEnd);
  //    float aspect = (float)width/height;
  //    projection = glm::ortho(-aspect, aspect, -1.f, 1.f, 0.1f, 400.f);
}

void Camera::setDrawDistance(float dist) { drawEnd = dist; }

glm::vec3 Camera::getPosition() const { return cameraPos; }

glm::vec3 Camera::getDirection() const { return cameraDirection; }

glm::vec3 Camera::getCameraUp() const { return cameraUp; }

glm::vec3 Camera::getCameraRight() const { return cameraRight; }
