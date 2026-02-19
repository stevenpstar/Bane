#include "bane/components/lightdata.hpp"
#include <bane/components/camera.hpp>
#include <bane/primitives/mesh.hpp>
#include <bane/primitives/texture.hpp>
#include <bane/primitives/vertex.hpp>
#include <bane/utility/shader.hpp>
#include <cmath>
#include <glad/glad.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#include <string>
#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

Mesh::Mesh(std::vector<Vertex> verts, std::vector<unsigned int> ind, std::vector<Texture> tex) {
  vertices = verts;
  indices = ind;
  textures = tex;
}

void Mesh::setupMesh() {
  //  std::cout << "vertices: \n";
  //  for (int i = 0; i < vertices.size(); ++i) {
  //    for (int j = 0; j < 4; ++j) {
  //      std::cout << vertices[i].BoneIds[j] << "\n";
  //    }
  //    std::cout << "--\n";
  //  }
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

  if (indices.size() == 0) {
    std::cout << "No indices found!\n";
    return;
  }

  //  for (const auto &vert : vertices) {
  //    for (const auto &boneId : vert.Weights) {
  //      std::cout << "bone id: " << boneId << "\n";
  //    }
  //  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Normal));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, TexCoords));
  glEnableVertexAttribArray(3);
  glVertexAttribIPointer(3, 4, GL_INT, sizeof(Vertex), (void *)offsetof(Vertex, BoneIds));
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Weights));
  // glEnableVertexAttribArray(5);
  // glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Tangent));
}

void Mesh::Render(glm::vec3 pos, glm::vec3 rotation, Shader *shader, Camera *camera) {
  std::cout << "Am I ever actually calling this?\n";
  shader->use();
  unsigned int diffuseNr = 1;
  unsigned int specularNr = 1;
  unsigned int normalNr = 1;
  for (unsigned int i = 0; i < textures.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    std::string number;
    std::string name = textures[i].type;

    if (name == "texture_diffuse") {
      number = std::to_string(diffuseNr++);
    }
    // else if (name == "texture_specular") {
    //   number = std::to_string(specularNr++);
    // } else if (name == "texture_normal") {
    //   number = std::to_string(normalNr++);
    // }

    shader->setInt(("material." + name + number).c_str(), i);

    glBindTexture(GL_TEXTURE_2D, textures[i].id);
  }
  glm::mat4 model = glm::mat4(1.f);
  model = glm::translate(model, pos);
  glm::mat4 rot = glm::mat4(1.f);
  rot = glm::rotate(rot, atan2(rotation.x, rotation.z), glm::vec3(0.f, 1.f, 0.f));
  glm::mat4 rotX = glm::mat4(1.f);
  rotX = glm::rotate(rot, atan2(rotation.y, rotation.z), glm::vec3(1.f, 0.f, 0.f));
  // model = model * rotX;
  // model = glm::rotate(model, atan2(rotation.x, rotation.z), glm::vec3(0.f, 1.f, 0.f));

  std::cout << "camera direction: " << glm::to_string(camera->getDirection()) << "\n";
  shader->setMat4("model", model);
  shader->setMat4("view", camera->getTransform());
  shader->setMat4("projection", camera->projection);
  shader->setVec3("viewPos", camera->getPosition());
  shader->setVec3("camDir", camera->getDirection());
  shader->setVec3("objectColour", glm::vec3(1.f, 1.f, 1.f));
  shader->setVec3("dirLight.direction", glm::vec3(5.f, 2.f, 2.f));
  shader->setVec3("material.ambient", glm::vec3(0.f, 0.f, 0.f));
  shader->setFloat("material.shininess", 32.f);
  shader->setVec3("dirLight.ambient", glm::vec3(0.75f, 0.75f, 0.75f));
  shader->setVec3("dirLight.diffuse", glm::vec3(1.f, 1.f, 1.f));
  shader->setVec3("dirLight.specular", glm::vec3(1.f, 1.f, 1.f));
  glActiveTexture(GL_TEXTURE0);

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  // if (glGetError() != 0) {
  //   std::cout << "gl error: " << glGetError() << "\n";
  // }
}

// Renders the mesh only, no shaders or uniforms set.
void Mesh::RenderBasic() {
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::Render(glm::mat4 transform, Shader *shader, Camera *camera, LightData *lightData, unsigned int shadowTex,
                  unsigned int normalTex) {
  shader->use();

  unsigned int diffuseNr = 1;
  unsigned int specularNr = 1;

  glm::vec3 normDir = glm::normalize(camera->getDirection());
  // testing random point lights (starting with 1)
  glm::vec3 p1Pos = glm::vec3(0.f, 2.f, 2.f);
  glm::vec3 p1Diff = glm::vec3(0.0, 0.0, 1.0);

  glm::vec3 p2Pos = glm::vec3(2.f, 1.f, 2.f);
  glm::vec3 p2Diff = glm::vec3(1.0, 0.0, 1.0);

  float p1Const = 1.f;
  float p1Linear = 0.09f;
  float p1Quad = 0.032f;

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textures[0].id);

  shader->setMat4("model", transform);
  shader->setMat4("view", camera->getTransform());
  shader->setMat4("projection", camera->projection);
  shader->setVec3("viewPos", camera->getPosition());
  shader->setVec3("camDir", camera->getDirection());
  shader->setVec3("objectColour", glm::vec3(1.f, 1.f, 1.f));
  shader->setVec3("lightPos", glm::vec3(-2.f, 4.f, -4.f));
  shader->setFloat("ambientStrength", 0.2);
  //  shader->setInt("normalMap", 2);
  shader->setInt("shadowMap", 1);
  shader->setInt("diffuseTexture", 0);

  // Testing point light
  // for (int i = 0; i < 1; ++i) {
  shader->setVec3("pointLights[0].position", p1Pos);
  shader->setVec3("pointLights[0].diffuse", p1Diff);
  shader->setFloat("pointLights[0].constant", p1Const);
  shader->setFloat("pointLights[0].linear", p1Linear);
  shader->setFloat("pointLights[0].quadratic", p1Quad);
  shader->setFloat("pointLights[0].intensity", 50.f);

  shader->setVec3("pointLights[1].position", p2Pos);
  shader->setVec3("pointLights[1].diffuse", p2Diff);
  shader->setFloat("pointLights[1].constant", p1Const);
  shader->setFloat("pointLights[1].linear", p1Linear);
  shader->setFloat("pointLights[1].quadratic", p1Quad);
  shader->setFloat("pointLights[1].intensity", 10.f);
  // }
  //

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, shadowTex);

  // glActiveTexture(GL_TEXTURE2);
  // glBindTexture(GL_TEXTURE_2D, normalTex);

  auto error = glGetError();
  if (error != GL_NO_ERROR)
    throw std::runtime_error("OpenGL error: " + std::to_string(error));

  glBindVertexArray(VAO);
  error = glGetError();
  if (error != GL_NO_ERROR)
    throw std::runtime_error("OpenGL error: " + std::to_string(error));

  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  error = glGetError();
  if (error != GL_NO_ERROR)
    throw std::runtime_error("OpenGL error: " + std::to_string(error));
}
