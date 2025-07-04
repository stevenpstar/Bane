#ifndef BANE_ENGINE
#define BANE_ENGINE
#include "bane/utility/shader.hpp"
#include <string>
class GLFWwindow;

void Print(std::string message);
GLFWwindow* CreateWindow();
bool ShouldClose(GLFWwindow* window);
void Poll();
void ClearColour();
void SwapBuffer(GLFWwindow* window);
void DestroyBane();
void RenderColouredTriangle(Shader* shader, std::array<float, 18> &triangle);
#endif
