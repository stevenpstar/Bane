#include "bane/utility/shader.hpp"
#include "glm/fwd.hpp"
#include <bane/components/bobject.hpp>
#include <bane/components/camera.hpp>
#include <glm/glm.hpp>

Bobject::Bobject(std::string objName, int id)
    : name(objName), id(id), editorCollision(AABB(glm::vec3(0.f, 0.f, 0.f), 2.f, 2.f, 2.f, this, objName)) {}
void Bobject::Render(Camera *camera) {}
void Bobject::Render(glm::mat4 transform, Shader *shader, Camera *camera, unsigned int shadowTex) {}
void Bobject::RenderBasic() {}
void Bobject::Update(float dt) { editorCollision.origin = transform.position; }
void Bobject::setPosition(glm::vec3 position) {
  transform.position = position;
  editorCollision.origin = transform.position;
}
