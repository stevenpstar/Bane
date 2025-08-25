#ifndef BANE_CHARACTER
#define BANE_CHARACTER
#include <bane/components/bobject.hpp>
#include <bane/primitives/animatedModel.hpp>
#include <bane/utility/AABB.hpp>
class Shader;
class Character : Bobject {
private:
public:
  Shader *shader;
  Character(const char *modelPath, Shader *shader);
  AnimatedModel model;
  AABB collisionBox;
  virtual void Render(Camera *camera) override;
  virtual void Update(float deltaTime);
  AnimatedModel *getModel();
  bool alive = true;
  float randomColour = 0.7f;
  float pathTimer = 100.f;
  float pathTicker = 0.0f;
  glm::vec3 targetLocation = glm::vec3(0.f, 0.f, 0.f);
};
#endif
