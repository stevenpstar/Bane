#ifndef BANE_STATIC_MESH
#define BANE_STATIC_MESH
#include "bane/primitives/model.hpp"
#include "bane/utility/shader.hpp"
#include <bane/components/bobject.hpp>
class StaticMesh: public Bobject
{
  private:
    Model* model;
    unsigned int textureID;
    Shader* shader;
  public:
  StaticMesh(Model* m, Shader* shdr);
  void Render() override;
};
#endif
