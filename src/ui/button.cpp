#include "glm/ext/matrix_clip_space.hpp"
#include <functional>
#include <glm/ext/matrix_transform.hpp>
#include <bane/components/camera.hpp>
#include <bane/primitives/texture.hpp>
#include <bane/ui/button.hpp>
#include <bane/utility/shader.hpp>
#include <glad/glad.h>

Button::Button(unsigned int textureId, Shader* shdr, float x, float y, float width, float height)
{
  this->x = x;
  this->y = y;
  this->width = width;
  this->height = height;
//    this->vertices = {
//        // positions          // colors           // texture coords
//         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
//         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
//        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
//        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
//    };
    this->vertices = {
        // positions          // colors           // texture coords
         x + width, y, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         x + width, y + height, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        x, y + height, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        x,  y, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };

    indices = {  
          0, 1, 3, // first triangle
          1, 2, 3  // second triangle
    };

    shader = shdr;
    this->textureId = textureId;
}

void Button::AttachShader(Shader* shdr)
{
  shader = shdr;
}

void Button::UpdateVertices(float x, float y, float width, float height) 
{
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;

    this->vertices = {
        // positions          // colors           // texture coords
        x + width, y, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
        x + width, y + height, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        x, y + height, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        x,  y, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };


    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void Button::AttachTexture(const char* path)
{
  textureId = createTexture(path);
}

void Button::SetupButton()
{

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
  
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  glBindVertexArray(0);

}

void Button::Render(Camera* camera, Shader* shdr, int width, int height)
{
  glm::mat4 model = glm::mat4(1.f);
  double aspect = (double)height/(double)width;
  double w = (double)width;
  double h = (double)height;
  glm::mat4 ortho = glm::ortho(0.0, w, 0.0, h, -1.0, 1.0);
  //ortho = glm::scale(ortho, glm::vec3(0.025f, 0.025f, 1.f));
  shader->use();
  shader->setMat4("model", model);
  shader->setMat4("projection", ortho);
  glBindTexture(GL_TEXTURE_2D, textureId);
//  std::cout << "Error: " << glGetError() << "\n";
  glActiveTexture(GL_TEXTURE0);
 // std::cout << "Error active: " << glGetError() << "\n";
  shdr->setInt("uitexture", 0);

  glBindVertexArray(VAO);
  //std::cout << "Error VAO?: " << glGetError() << "\n";
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  //std::cout << "Error DrawElems?: " << glGetError() << "\n";
  glBindVertexArray(0);
}

bool Button::IsHovered(double mx, double my)
{
  return mx >= x && mx <= x + width &&
    my >= y && my <= y + height;
}

void Button::Activate()
{
  activateCallback();
}
