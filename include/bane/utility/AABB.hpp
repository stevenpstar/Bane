#ifndef BANE_AABB
#define BANE_AABB

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <string>
class Camera;
class Shader;
class Bobject;
class AABB {
public:
  AABB(glm::vec3 origin, float xdim, float ydim, float zdim, Bobject *owner, std::string name, int id);
  std::string name;
  int id;
  glm::vec3 origin;
  float xdim;
  float ydim;
  float zdim;
  unsigned int VAO;
  unsigned int VBO;
  Bobject *owner;
  void setupAABB();
  void render(Camera *camera, Shader *shader);
  bool isColliding(AABB *aabb);
};

float AABB_getMinX(AABB *aabb);
float AABB_getMaxX(AABB *aabb);

float AABB_getMinY(AABB *aabb);
float AABB_getMaxY(AABB *aabb);

float AABB_getMinZ(AABB *aabb);
float AABB_getMaxZ(AABB *aabb);

#endif
