#include <algorithm>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <bane/utility/ray.hpp>
#include <bane/utility/shader.hpp>
#include <bane/components/camera.hpp>
#include <iostream>
#include <glad/glad.h>

bool rayWithWorldPlane(glm::vec3 rayDir, glm::vec3 rayOrigin, float offset, glm::vec3 &hitPoint)
{
  float nd = glm::dot(rayDir, glm::vec3(0.f, 1.f, 0.f));
  float pn = glm::dot(rayOrigin, glm::vec3(0.f, 1.f, 0.f));

  float distance = glm::distance(rayOrigin, glm::vec3(0.f, 0.f, 0.f));
  float top = glm::dot(rayOrigin, glm::vec3(0.f, 1.f, 0.f)) + 0.f;
  float bot = glm::dot(rayDir, glm::vec3(0.f, 1.f, 0.f));
  float t = -(top/bot);
  if (t >= 0.f)
  {
    hitPoint = rayOrigin + rayDir * t;
    return true;
  } 
  return false;
}

bool rayWithAABB(AABB* aabb, glm::vec3 rayDir, glm::vec3 rayOrigin, glm::vec3& hitPoint)
{

  float xmin = AABB_getMinX(aabb);
  float xmax = AABB_getMaxX(aabb);
  float ymin = AABB_getMinY(aabb);
  float ymax = AABB_getMaxY(aabb);
  float zmin = AABB_getMinZ(aabb);
  float zmax = AABB_getMaxZ(aabb);

  if (rayDir.x == 0.f)
  {
    rayDir.x = 0.00001f;
  }
  float t1 = (xmin - rayOrigin.x) / rayDir.x;
  float t2 = (xmax - rayOrigin.x) / rayDir.x;
  if (t1 > t2)
  {
    float temp = 0.f;
    temp = t1;
    t1 = t2;
    t2 = temp;
  }
  if (rayDir.y == 0.f)
  {
    rayDir.y = 0.00001f;
  }

  float t3 = (ymin - rayOrigin.y) / rayDir.y;
  float t4 = (ymax - rayOrigin.y) / rayDir.y;

  if (t3 > t4)
  {
    float temp = 0.f;
    temp = t3;
    t3 = t4;
    t4 = temp;
  }

  if (rayDir.z == 0.f)
  {
    rayDir.z = 0.00001f;
  }

  float t5 = (zmin - rayOrigin.z) / rayDir.z;
  float t6 = (zmax - rayOrigin.z) / rayDir.z;

  if (t5 > t6)
  {
    float temp = 0.f;
    temp = t5;
    t5 = t6;
    t6 = temp;
  }

  float tMin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
  float tMax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

  if (tMax < 0) 
  {
    return false;
  }

  if (tMin > tMax)
  {
    return false;
  }

  return true;
}

glm::vec3 getRayFromMouse(Camera* camera, int width, int height, double xpos, double ypos)
{
  float x = (2.f * xpos) / width - 1.f;
  float y = 1.f - (2.f * ypos) / height;
  float z = 1.f;

  glm::vec4 nds_ray = glm::vec4(x, y, -1.f, 1.f);
  glm::vec4 ray_eye = glm::inverse(camera->projection) * nds_ray;
  ray_eye.z = -1.f;
  ray_eye.w = 0.f;
  glm::vec4 vec4_world = glm::inverse(camera->getTransform()) * ray_eye;
  glm::vec3 ray_pos = glm::vec3(vec4_world.x, vec4_world.y, vec4_world.z);
  glm::vec3 world_ray = glm::normalize(glm::vec3(vec4_world.x, vec4_world.y, vec4_world.z));
  glm::vec3 dir = world_ray;

  return dir;
}

void renderRay(glm::vec3 rayOrigin, glm::vec3 rayDir, Camera *camera, Shader *shader)
{
  unsigned int VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glm::vec3 endPoint = rayOrigin + rayDir * 100.f;

  float vertices[] = { 
    rayOrigin.x, rayOrigin.y, rayOrigin.z,
    endPoint.x, endPoint.y, endPoint.z,
  };

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER,
        6 * sizeof(float),
        vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  shader->use();
  shader->setMat4("model", glm::mat4(1.f));
  shader->setMat4("view", camera->getTransform());
  shader->setMat4("projection", camera->projection);

  glDrawArrays(GL_LINES, 0, 2);

  glEnableVertexAttribArray(0);
}

