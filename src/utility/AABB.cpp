#include "glm/ext/matrix_transform.hpp"
#include <bane/components/bobject.hpp>
#include <bane/components/camera.hpp>
#include <bane/utility/AABB.hpp>
#include <bane/utility/shader.hpp>
#include <glad/glad.h>
#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

AABB::AABB(glm::vec3 origin, float xdim, float ydim, float zdim, Bobject *owner, std::string name, int id) {
  this->origin = origin;
  this->xdim = xdim;
  this->ydim = ydim;
  this->zdim = zdim;
  this->owner = owner;
  this->name = name;
  this->id = id;

  setupAABB();
}

void AABB::setupAABB() {
  // 108
  float vertices[] = {
      -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, -0.5f,

      -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,

      -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,

      0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,

      -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f,

      -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f};
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

  glBindVertexArray(0);
}

void AABB::render(Camera *camera, Shader *shader) {
  shader->use();

  glm::mat4 model = glm::mat4(1.f);
  model = glm::translate(model, origin);
  model = glm::scale(model, glm::vec3(xdim, ydim, zdim));
  shader->setMat4("model", model);
  shader->setMat4("view", camera->getTransform());
  shader->setMat4("projection", camera->projection);

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

bool AABB::isColliding(AABB *aabb) {
  float xmin = AABB_getMinX(this);
  float xmax = AABB_getMaxX(this);
  float ymin = AABB_getMinY(this);
  float ymax = AABB_getMaxY(this);
  float zmin = AABB_getMinZ(this);
  float zmax = AABB_getMaxZ(this);

  float b_xmin = AABB_getMinX(aabb);
  float b_xmax = AABB_getMaxX(aabb);
  float b_ymin = AABB_getMinY(aabb);
  float b_ymax = AABB_getMaxY(aabb);
  float b_zmin = AABB_getMinZ(aabb);
  float b_zmax = AABB_getMaxZ(aabb);

  return xmin <= b_xmax && xmax >= b_xmin && ymin <= b_ymax && ymax >= b_ymin && zmin <= b_zmax && zmax >= b_zmin;
}

float AABB_getMinX(AABB *aabb) {
  float minx = aabb->origin.x - aabb->xdim / 2.f;
  return minx;
}
float AABB_getMaxX(AABB *aabb) {
  float maxx = aabb->origin.x + aabb->xdim / 2.f;
  return maxx;
}

float AABB_getMinY(AABB *aabb) { return aabb->origin.y - aabb->ydim / 2.f; }
float AABB_getMaxY(AABB *aabb) { return aabb->origin.y + aabb->ydim / 2.f; }

float AABB_getMinZ(AABB *aabb) { return aabb->origin.z - aabb->zdim / 2.f; }
float AABB_getMaxZ(AABB *aabb) { return aabb->origin.z + aabb->zdim / 2.f; }
