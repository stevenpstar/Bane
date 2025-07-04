#ifndef BANE_ICONTROLLER
#define BANE_ICONTROLLER

struct GLFWwindow;
class IController
{
  public:
    virtual void processInput(GLFWwindow* window);
    virtual void update(float deltaTime);
    virtual void processMouse(GLFWwindow* window, double xpos, double ypos);
    virtual void Enable();
    virtual void Disable();
};
#endif
