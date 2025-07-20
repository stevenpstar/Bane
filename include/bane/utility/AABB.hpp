#ifndef BANE_AABB
#define BANE_AABB

#include <glm/fwd.hpp>
#include <bane/components/bobject.hpp>
#include <glm/glm.hpp>
class Camera;
class Shader;
class AABB
{
  public:
  AABB(glm::vec3 origin, float xdim, float ydim, float zdim, Bobject* owner);
  glm::vec3 origin;
  float xdim;
  float ydim;
  float zdim;
  unsigned int VAO;
  unsigned int VBO;
  Bobject* owner;
  void setupAABB();
  void render(Camera* camera, Shader* shader);
  bool isColliding(AABB* aabb);
};

float AABB_getMinX(AABB* aabb);
float AABB_getMaxX(AABB* aabb);

float AABB_getMinY(AABB* aabb);
float AABB_getMaxY(AABB* aabb);

float AABB_getMinZ(AABB* aabb);
float AABB_getMaxZ(AABB* aabb);

#endif
