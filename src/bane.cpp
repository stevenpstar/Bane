#include <bane/utility/shader.hpp>
#include <bane/bane.hpp>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stbimage/stb_image.h>
#include <array>

void Print(std::string message)
{
  std::cout << message << std::endl;
}

void framebuffer_size_callback(__attribute__((unused)) GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
}

void SetCursorCallback(GLFWwindow* window, void(*f)(GLFWwindow* window, double xpos, double ypos))
{
  glfwSetCursorPosCallback(window, f);
}

void SetResizeCallback(GLFWwindow *window, void (*f)(GLFWwindow *, int, int))
{
  glfwSetFramebufferSizeCallback(window, f);
}

void ResizeViewport(int width, int height)
{
  glViewport(0, 0, width, height);
}

void CaptureMouse(GLFWwindow* window)
{
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

GLFWwindow* CreateWindow()
{
  std::cout << "Trying to create window!\n";
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(800, 600, "Bane Engine", NULL, NULL);
  if (window == NULL)
  {
    std::cout<< "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    throw std::runtime_error("Failed to create GLFW window");
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
 // stbi_set_flip_vertically_on_load(true);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout<<"Failed to init GLAD"<< std::endl;
    throw std::runtime_error("Failed to initialise Glad!");
  }

  glViewport(0, 0, 800, 600);
  glEnable(GL_DEPTH_TEST);

  // some setup, should be moved to own function(s)
  unsigned int VAO, VBO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  return window;
}

float GetTime()
{
  return glfwGetTime();
}

void Poll()
{
  glfwPollEvents();
}

void ClearColour()
{
  glClearColor(1.f, 1.f, 1.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SwapBuffer(GLFWwindow* window)
{
  glfwSwapBuffers(window);
}

void DestroyBane()
{
  glfwTerminate();
}

bool ShouldClose(GLFWwindow* window)
{
  return glfwWindowShouldClose(window);
}

// Test function
void RenderColouredTriangle(Shader* shader, std::array<float, 18> &triangle)
{
  if (triangle.size() == 0) { return; }
  glBufferData(GL_ARRAY_BUFFER, triangle.size() * sizeof(float), triangle.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  shader->use();
  glDrawArrays(GL_TRIANGLES, 0, 3);
}
