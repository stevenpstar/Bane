#include <bane/components/camera.hpp>
#include <bane/primitives/texture.hpp>
#include <bane/utility/shader.hpp>
#include <glm/fwd.hpp>
#include <bane/primitives/skybox.hpp>
#include <glad/glad.h>
Skybox::Skybox(const char* folderPath) :
  path(folderPath)
{
  texture = createSkyBoxTexture(path);

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glBufferData(GL_ARRAY_BUFFER, sizeof(cube), &cube, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

  glBindVertexArray(0);
}

void Skybox::Render(Camera* camera, Shader* shader)
{
  glm::mat4 view = glm::mat4(glm::mat3(camera->getTransform()));
  shader->use();
  glDepthFunc(GL_LEQUAL);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
  shader->setInt("skybox", 0);
  shader->setMat4("view", view);
  shader->setMat4("projection", camera->projection);
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glDepthFunc(GL_LESS);
}
