// Bane Object = Bobject
#ifndef BANE_BOBJECT
#define BANE_BOBJECT
#include <bane/primitives/transform.hpp>
#include <bane/utility/AABB.hpp>
#include <glm/glm.hpp>
#include <string>
class Camera;
class Shader;
class Bobject {
private:
public:
  int id;
  unsigned int textureId;
  std::string name;
  AABB editorCollision;
  Transform transform;
  bool castShadow = false;
  Bobject(std::string objName, int id);
  virtual void Render(Camera *camera);
  virtual void Render(glm::mat4 transform, Shader *shader, Camera *camera, unsigned int shadowTex);
  virtual void RenderBasic();
  virtual void Update(float dt);
  virtual void setPosition(glm::vec3 position);
};
#endif
