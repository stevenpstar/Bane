#ifndef BANE_STATIC_MESH
#define BANE_STATIC_MESH
#include <bane/components/bobject.hpp>
class Camera;
class Shader;
class Model;
class AABB;
class StaticMesh: public Bobject
{
  private:
    Model* model;
    unsigned int textureID;
    Shader* shader;
  public:
  StaticMesh(Model* m, Shader* shdr);
  void Render(Camera* camera) override;
};
#endif
