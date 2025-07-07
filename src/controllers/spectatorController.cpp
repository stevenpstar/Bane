#include <GLFW/glfw3.h>
#include <bane/controllers/spectatorController.hpp>
#include <bane/components/camera.hpp>
#include <iostream>

SpectatorController::SpectatorController(Camera* cam)
{
  camera = cam;
}

void SpectatorController::processInput(__attribute__((unused)) GLFWwindow* window)
{
  if (disabled) { return; }
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
  {
    forwardDown = true;
  } else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE) {
    forwardDown = false;
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
  {
    backDown = true;
  } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE)
  {
    backDown = false;
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
  {
    leftDown = true;
  } else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE)
  {
    leftDown = false;
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
  {
    rightDown = true;
  } else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE)
  {
    rightDown = false;
  }
}

void SpectatorController::processMouse(GLFWwindow*, double xpos, double ypos)
{
  const float sensitivity = 0.1f;
  if (initialising)
  {
    m_lastX = xpos;
    m_lastY = ypos;
    initialising = false;
    pitch = 0.f;
    yaw = 0.f;
    return;
  }

  if (disabled) { 
    m_lastX = xpos;
    m_lastY = ypos;
    return; }

  float xOffset = xpos - m_lastX;
  float yOffset = m_lastY - ypos;
  m_lastX = xpos;
  m_lastY = ypos;

  xOffset *= sensitivity;
  yOffset *= sensitivity;

  yaw += xOffset;
  pitch += yOffset;

  if (pitch > 89.f)
  {
    pitch = 89.f;
  } else if (pitch < -89.f)
  {
    pitch = -89.f;
  }

  glm::vec3 dir;
  dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  dir.y = sin(glm::radians(pitch));
  dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  camera->setCameraDirection(glm::normalize(dir));
}

void SpectatorController::update(float deltaTime)
{
  if (disabled) { return; }
  float forward = 0.f;
  float left = 0.f;
  if (forwardDown) {
    forward += 1.f;
  }
  if (backDown) {
    forward -= 1.f;
  }

  if (leftDown) {
    left += 1.f;
  } 
  if (rightDown) {
    left -= 1.f;
  }
  moveVector = glm::vec3(0.f, 0.f, 0.f);
  moveVector += camera->getDirection() * forward;
  moveVector += camera->getCameraRight() * left;
  camera->setPosition(camera->getPosition() + moveVector * cameraSpeed * deltaTime);
  camera->lookAtTarget(camera->getPosition() + camera->getDirection());
}

void SpectatorController::Enable()
{
  disabled = false;
}

void SpectatorController::Disable()
{
  disabled = true;
}
