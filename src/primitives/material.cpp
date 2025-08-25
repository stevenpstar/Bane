#include <bane/primitives/material.hpp>
#include <bane/utility/shader.hpp>
#include <glad/glad.h>
void AssignMaterialUnif(Material *material, Shader *shader) {
  shader->use();
  shader->setBool("lightRim", material->useRimLighting);
  shader->setBool("reflective", material->reflective);
  // assign texture ids (subject to change)
  // 0 = Diffuse
  // 1 = Shadow
  // 2 = Skybox
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, material->diffuseTextureId);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, material->shadowMapTextureId);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_CUBE_MAP, material->cubeMapTextureId);
  shader->setInt("diffuseTexture", 0);
  shader->setInt("shadowMap", 1);
  shader->setInt("reflectionMap", 2); // cube map/skybox
  // Set shine amount when that is a uniform/spec amount
}
