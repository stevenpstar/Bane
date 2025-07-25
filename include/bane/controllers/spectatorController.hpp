#ifndef BANE_SPEC_CONTROLLER
#define BANE_SPEC_CONTROLLER
#include <bane/controllers/icontroller.hpp>
#include <glm/glm.hpp>
struct GLFWwindow;
class Camera;
class Character;
class SpectatorController : public IController
{
  private:
    Camera* camera;
    bool forwardDown = false;
    bool rightDown = false;
    bool leftDown = false;
    bool backDown = false;
    float cameraSpeed = 2.5f;
    glm::vec3 moveVector = glm::vec3(0.f, 0.f, 0.f);
    float m_lastX;
    float m_lastY;
    float pitch;
    float yaw;
    bool initialising = true;
    bool disabled = false;
  public:
    SpectatorController(Camera* camera);
    Character* character;
    bool captured = false;
    virtual void processInput(GLFWwindow* window) override;
    virtual void processMouse(GLFWwindow* window, double xpos, double ypos) override;
    virtual void processMouseInput(GLFWwindow* window, int button, int action, int mods) override;
    virtual void update(float deltaTime) override;

    void centerMousePosition(int width, int height);
    virtual void Enable() override;
    virtual void Disable() override;
};
#endif
