#include <bane/components/camera.hpp>
#include <glm/glm.hpp>
#include <bane/primitives/instancedStaticMeshes.hpp>
#include <bane/primitives/model.hpp>
#include <bane/utility/shader.hpp>
#include <vector>
#include <glad/glad.h>

InstancedStaticMeshes::InstancedStaticMeshes(Model* model, Shader* shdr, std::vector<glm::mat4> transforms)
{
  this->model = model;
  this->shader = shdr;
  this->transforms = transforms;
 // this->model->meshes[0].setupMesh();
}

void InstancedStaticMeshes::Render(Camera* camera)
{
  shader->use();

  unsigned int diffuseNr = 1;
  unsigned int specularNr = 1;
  for (unsigned int i = 0; i < model->meshes[0].textures.size(); i++)
  {
    glActiveTexture(GL_TEXTURE0 + i);
    std::string number;
    std::string name = model->meshes[0].textures[i].type;

    if (name == "texture_diffuse")
    {
      number = std::to_string(diffuseNr++);
    } else if (name == "texture_specular")
    {
      number = std::to_string(specularNr++);
    }

    shader->setInt(("material." + name + number).c_str(), i);

    glBindTexture(GL_TEXTURE_2D, model->meshes[0].textures[i].id);
  }

  shader->setMat4("view", camera->getTransform());
  shader->setMat4("projection", camera->projection);
  shader->setVec3("viewPos", camera->getPosition());
  shader->setVec3("objectColour", glm::vec3(1.f, 1.f, 1.f));
  shader->setVec3("lightPosition", glm::vec3(5.f, 2.f, 2.f));
  shader->setVec3("dirLight.direction", glm::vec3(5.f, 2.f, 2.f));
  shader->setVec3("material.ambient", glm::vec3(1.f, 1.f, 1.f));
  shader->setFloat("material.shininess", 32.f);
  shader->setVec3("dirLight.ambient", glm::vec3(0.5f, 0.5f, 0.5f));
  shader->setVec3("dirLight.diffuse", glm::vec3(1.f, 1.f, 1.f));
  shader->setVec3("dirLight.specular", glm::vec3(1.f, 1.f, 1.f));
  for (int i = 0; i < transforms.size(); ++i)
  {
    // limiting to 100 for now
    if (i >= 100) {
      break;
    }
    shader->setMat4("model[" + std::to_string(i) + "]", transforms[i]);
  }
  glActiveTexture(GL_TEXTURE0);

  int count = transforms.size();
  if (count > 100)
  {
    count = 100;
  }

  glBindVertexArray(model->meshes[0].VAO);
  glDrawElementsInstanced(GL_TRIANGLES, model->meshes[0].indices.size(), 
      GL_UNSIGNED_INT, 0, count);
  glBindVertexArray(0);
}
