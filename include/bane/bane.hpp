#ifndef BANE_ENGINE
#define BANE_ENGINE
#include <bane/utility/shader.hpp>
#include <string>
struct GLFWwindow;

void Print(std::string message);
GLFWwindow* CreateWindow();
bool ShouldClose(GLFWwindow* window);
float GetTime();
void Poll();
void ClearColour();
void SwapBuffer(GLFWwindow* window);
void DestroyBane();
void SetCursorCallback(GLFWwindow* window, void(*f)(GLFWwindow* window, double xpos, double ypos));
void SetResizeCallback(GLFWwindow* window, void(*f)(GLFWwindow* window, int width, int height));
void SetMouseInputCallback(GLFWwindow* window, void(*f)(GLFWwindow* window, int button, int action, int mods));
void ResizeViewport(int width, int height);
void CaptureMouse(GLFWwindow* window);
void RenderColouredTriangle(Shader* shader, std::array<float, 18> &triangle);

#endif
