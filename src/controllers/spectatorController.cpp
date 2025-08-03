#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_stdinc.h>
#include <bane/utility/ray.hpp>
#include <bane/controllers/spectatorController.hpp>
#include <bane/components/camera.hpp>
#include <bane/components/character.hpp>

SpectatorController::SpectatorController(Camera* cam)
{
  camera = cam;
}

void SpectatorController::processInput(__attribute__((unused)) SDL_Window* window, Uint32 button, bool pressed)
{
  if (disabled) { return; }
 
  if (button == SDLK_W && pressed) {
    forwardDown = true;
  }
  else if (button == SDLK_W && !pressed) {
    forwardDown = false;
  }

  if (button == SDLK_S && pressed) {
    backDown = true;
  }
  else if (button == SDLK_S && !pressed) {
    backDown = false;
  }

  if (button == SDLK_A && pressed) {
    leftDown = true;
  }
  else if (button == SDLK_A && !pressed) {
    leftDown = false;
  }

  if (button == SDLK_D && pressed) {
    rightDown = true;
  }
  else if (button == SDLK_D && !pressed) {
    rightDown = false;
  }
}

void SpectatorController::processMouseInput(SDL_Window*, int button, int action, int mods)
{

}

void SpectatorController::processMouse(SDL_Window*, double xpos, double ypos)
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


  if (character) {
    //glm::vec3 rayDir = getRayFromMouse(camera, GLOBAL_WIDTH, GLOBAL_HEIGHT, xpos, ypos);
    //glm::vec3 hitPoint = glm::vec3(0.f, 0.f, 0.f);
    //if (rayWithAABB(character->collisionBox, ))
    //{
    //  character->model.position = hitPoint;
    //}

    if (character->model.currentAnimationIndex != 36)
    {
      character->model.PlayAnimation(36);
    }
  }
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
  //camera->lookAtTarget(glm::vec3(0.f, 0.f, 0.f));
}

void SpectatorController::Enable()
{
  disabled = false;
}

void SpectatorController::Disable()
{
  disabled = true;
}
