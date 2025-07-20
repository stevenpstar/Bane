#include <bane/components/camera.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
#include <cmath>
#include <glad/glad.h>
#include <bane/primitives/vertex.hpp>
#include <bane/primitives/texture.hpp>
#include <bane/primitives/mesh.hpp>
#include <bane/utility/shader.hpp>
#include <iostream>
#include <vector>
#include <string>

Mesh::Mesh(std::vector<Vertex> verts, std::vector<unsigned int> ind, std::vector<Texture> tex)
{
  vertices = verts;
  indices = ind;
  textures = tex;
}

void Mesh::setupMesh()
{
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);


  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER,
      vertices.size() * sizeof(Vertex),
      &vertices[0], GL_STATIC_DRAW);

  if (indices.size() == 0) {
    std::cout << "No indices found!\n";
    return;
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
      &indices[0], GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
  glEnableVertexAttribArray(3);
  glVertexAttribIPointer(3, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, BoneIds));
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Weights));

  glBindVertexArray(0);
}

void Mesh::Render(glm::vec3 pos, glm::vec3 rotation, Shader* shader, Camera* camera)
{
  shader->use();
  unsigned int diffuseNr = 1;
  unsigned int specularNr = 1;
  for (unsigned int i = 0; i < textures.size(); i++)
  {
    glActiveTexture(GL_TEXTURE0 + i);
    std::string number;
    std::string name = textures[i].type;

    if (name == "texture_diffuse")
    {
      number = std::to_string(diffuseNr++);
    } else if (name == "texture_specular")
    {
      number = std::to_string(specularNr++);
    }

    shader->setInt(("material." + name + number).c_str(), i);

    glBindTexture(GL_TEXTURE_2D, textures[i].id);
  }
  glm::mat4 model = glm::mat4(1.f);
  model = glm::translate(model, pos);
  glm::mat4 rot = glm::mat4(1.f);
  rot = glm::rotate(rot, atan2(rotation.x, rotation.z), glm::vec3(0.f, 1.f, 0.f));
 // model = model * rot;
  glm::mat4 rotX = glm::mat4(1.f);
  rotX = glm::rotate(rot, atan2(rotation.y, rotation.z), glm::vec3(1.f, 0.f, 0.f));
  model = model * rotX;
  //model = glm::rotate(model, atan2(rotation.x, rotation.z), glm::vec3(0.f, 1.f, 0.f));

  shader->setMat4("model", model);
  shader->setMat4("view", camera->getTransform());
  shader->setMat4("projection", camera->projection);
  shader->setVec3("viewPos", camera->getPosition());
  shader->setVec3("objectColour", glm::vec3(1.f, 1.f, 1.f));
//  shader.setVec3("lightPosition", glm::vec3(5.f, 2.f, 2.f));
  shader->setVec3("dirLight.direction", glm::vec3(5.f, 2.f, 2.f));
  shader->setVec3("material.ambient", glm::vec3(0.f, 0.f, 0.f));
  shader->setFloat("material.shininess", 32.f);
  shader->setVec3("dirLight.ambient", glm::vec3(0.5f, 0.5f, 0.5f));
  shader->setVec3("dirLight.diffuse", glm::vec3(1.f, 1.f, 1.f));
  shader->setVec3("dirLight.specular", glm::vec3(1.f, 1.f, 1.f));
  glActiveTexture(GL_TEXTURE0);

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void Mesh::Render(glm::mat4 transform, Shader* shader, Camera* camera)
{
  shader->use();
  unsigned int diffuseNr = 1;
  unsigned int specularNr = 1;
  for (unsigned int i = 0; i < textures.size(); i++)
  {
    glActiveTexture(GL_TEXTURE0 + i);
    std::string number;
    std::string name = textures[i].type;

    if (name == "texture_diffuse")
    {
      number = std::to_string(diffuseNr++);
    } else if (name == "texture_specular")
    {
      number = std::to_string(specularNr++);
    }

    shader->setInt(("material." + name + number).c_str(), i);

    glBindTexture(GL_TEXTURE_2D, textures[i].id);
  }

  shader->setMat4("model", transform);
  shader->setMat4("view", camera->getTransform());
  shader->setMat4("projection", camera->projection);
  shader->setVec3("viewPos", camera->getPosition());
  shader->setVec3("objectColour", glm::vec3(1.f, 1.f, 1.f));
  shader->setVec3("dirLight.direction", glm::vec3(5.f, 2.f, 2.f));
  shader->setVec3("material.ambient", glm::vec3(0.f, 0.f, 0.f));
  shader->setFloat("material.shininess", 32.f);
  shader->setVec3("dirLight.ambient", glm::vec3(0.4f, 0.4f, 0.4f));
  shader->setVec3("dirLight.diffuse", glm::vec3(1.f, 1.f, 1.f));
  shader->setVec3("dirLight.specular", glm::vec3(1.f, 1.f, 1.f));
  shader->setVec3("spotLight.position", camera->getPosition());
  shader->setVec3("spotLight.direction", camera->getDirection());
  shader->setVec3("spotLight.diffuse", glm::vec3(1.f, 0.f, 0.f));
  shader->setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5)));
  shader->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5)));
  glActiveTexture(GL_TEXTURE0);

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

