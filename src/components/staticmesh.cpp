#include <bane/components/bobject.hpp>
#include <bane/components/camera.hpp>
#include <bane/components/staticmesh.hpp>
#include <bane/primitives/model.hpp>
#include <bane/utility/shader.hpp>
#include <glm/glm.hpp>

StaticMesh::StaticMesh(const char *modelPath, std::string objName, int id) : model(Model(modelPath)), Bobject(objName, id) {}

void StaticMesh::Render(Camera *camera) { model.RenderBasic(); }

void StaticMesh::Render(glm::mat4 transform, Shader *shader, Camera *camera, unsigned int shadowTex, unsigned int normalTex) {
  model.Render(transform, shader, camera, shadowTex, normalTex);
}

void StaticMesh::RenderBasic() { model.RenderBasic(); }

void StaticMesh::Update(float dt) { Bobject::Update(dt); }
