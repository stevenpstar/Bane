#include <bane/primitives/pointLight.hpp>
#include <bane/utility/shader.hpp>
#include <string>
// there are always n number of point lights, so they will be
// assigned in an array.
void AssignPointLightUnif(PointLight *light, int index, Shader *shader) {
  shader->use();
  shader->setVec3("pointLights[" + std::to_string(index) + "].position", light->position);
  shader->setVec3("pointLights[" + std::to_string(index) + "].diffuse", light->diffuse);
  shader->setFloat("pointLights[" + std::to_string(index) + "].constant", light->constant);
  shader->setFloat("pointLights[" + std::to_string(index) + "].linear", light->linear);
  shader->setFloat("pointLights[" + std::to_string(index) + "].quadratic", light->quadratic);
  shader->setFloat("pointLights[" + std::to_string(index) + "].intensity", light->intensity);
}
