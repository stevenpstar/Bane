#include <glm/geometric.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/trigonometric.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include<glm/gtx/string_cast.hpp>
#include <bane/primitives/animatedModel.hpp>
#include <bane/components/character.hpp>
#include <bane/components/camera.hpp>
#include <bane/controllers/testCharController.hpp>
#include <GLFW/glfw3.h>
#include <cmath>

void TestCharController::processInput(__attribute__((unused)) GLFWwindow* window)
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

void TestCharController::processMouse(GLFWwindow*, double xpos, double ypos)
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

  float radius = 4.5f;
  if (character) {
    glm::vec3 dir;
    dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    dir.x *= radius;
   // dir.y = sin(glm::radians(pitch));
    dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    dir.z *= radius;
    //camera->setCameraDirection(glm::normalize(dir));
    camera->setPosition(glm::vec3(character->model.position.x + dir.x, 3.5f, character->model.position.z + dir.z));
  }

// float camX = sin(glfwGetTime()) * radius;
//    float camZ = cos(glfwGetTime()) * radius;
//    camera->setPosition(glm::vec3(pos.x + camX, 2.f, pos.z + camZ));
}

void TestCharController::setCharacter(Character* chr)
{
  character = chr;
  character->model.boneRotations.push_back({
      "spine",
      0.f,
      glm::vec3(0.f, 0.f, 1.f)
      }
      );
  character->model.boneRotations.push_back({
      "head",
      0.f,
      glm::vec3(0.f, 1.f, 0.f)
  });
}

TestCharController::TestCharController(Camera* camera)
{
  this->camera = camera;
}

void TestCharController::update(float deltaTime)
{

  float radius = 4.5f;
  if (character) {
    glm::vec3 dir;
    dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    dir.x *= radius;
   // dir.y = sin(glm::radians(pitch));
    dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    dir.z *= radius;
    //camera->setCameraDirection(glm::normalize(dir));
    camera->setPosition(glm::vec3(character->model.position.x + dir.x, 3.5f, character->model.position.z + dir.z));
  }

  if (character) {
    glm::vec3 pos = character->model.position;
       float forward = 0.f;
    float left = 0.f;
    if (forwardDown) {
      forward += 1.f;
    }
    if (backDown) {
      forward -= 1.f;
    }
    if ((forwardDown || backDown) && character->model.currentAnimationIndex != 48)
    {
      character->model.PlayAnimation(48);
    } else if (!forwardDown && !backDown && character->model.currentAnimationIndex != 36)  {
      character->model.PlayAnimation(36);
    }
    if (leftDown) {
      left += 1.f;
    } 
    if (rightDown) {
      left -= 1.f;
    }
    float tilt = 0.f;
    for (auto& boneRot: character->model.boneRotations)
    { 
      if (boneRot.BoneName == "spine") {
        tilt = boneRot.Angle;
      }
    }

    float maxTilt = 25.f;
    if (!leftDown && !rightDown && tilt != 0.f) {
      if (tilt > 0.f) {
        tilt -= 1.f;
      } else {
        tilt += 1.f;
      }
    } else {
      tilt += -left * 1.f;
      if (tilt > maxTilt) {
        tilt = maxTilt;
      }
      if (tilt < -maxTilt) {
        tilt = -maxTilt;
      }
    }
    for (auto& boneRot: character->model.boneRotations)
    { 
      if (boneRot.BoneName == "spine") {
        boneRot.Angle = tilt;
      }
    }
   // pos.x += left * movementSpeed * deltaTime;
   // pos.z += forward * movementSpeed * deltaTime;
    glm::vec3 moveVector = glm::vec3(0.f ,0.f, 0.f);
    float turnAngle = 2.f;
    float speed = 4.f;
    character->model.rotation = glm::rotate(character->model.rotation, glm::radians(left * turnAngle), glm::vec3(0.f, 1.f, 0.f));
    glm::vec3 dir = glm::eulerAngles(glm::quat_cast(character->model.rotation));
    std::cout << "Dir: " << glm::to_string(dir) << "\n";
    character->model.modelDir = dir;
    glm::vec3 forwardDir = glm::vec3(character->model.rotation[2].x, character->model.rotation[2].y, character->model.rotation[2].z);
    character->model.modelRight = glm::cross(glm::normalize(forwardDir), glm::vec3(0.f, 1.f, 0.f));
    moveVector += forward * speed * (glm::vec3(character->model.rotation[2].x, character->model.rotation[2].y, character->model.rotation[2].z)) * deltaTime;
    glm::vec3 nextPos = character->model.position + glm::vec3(moveVector.x, 0.f, moveVector.z);
    bool colliding = false;
    character->collisionBox.origin = nextPos;
    for (int i=0;i<collisionBoxes->size();++i)
    {
      AABB* box = &collisionBoxes->at(i);
      if (this->character->collisionBox.isColliding(box))
      {
      colliding = true;
      }
    }
    if (!colliding) {
      character->model.position += glm::vec3(moveVector.x, 0.f, moveVector.z);
    }
   // std::cout<<glm::to_string(character->model.modelRight) << "\n";
   // std::cout<<glm::to_string(character->model.rotation) << "\n";
    camera->lookAtTarget(glm::vec3(character->model.position.x, character->model.position.y + 1.5f, character->model.position.z));
  } else {
    camera->lookAtTarget(glm::vec3(0.f, 0.f, 0.f));
  }
}

void TestCharController::setCameraPosition(glm::vec3 pos)
{
  camera->setPosition(pos);
}

void TestCharController::setMovementSpeed(float speed)
{
  movementSpeed = speed;
}

void TestCharController::Enable()
{
  disabled = false;
}

void TestCharController::Disable()
{
  disabled = true;
}

