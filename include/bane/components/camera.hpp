#ifndef BANE_CAMERA
#define BANE_CAMERA
#include <glm/glm.hpp>
class Camera
{
  public:
    Camera(glm::vec3 position);
    glm::mat4 lookAtTarget(glm::vec3 target);
    glm::mat4 getTransform();
    glm::mat4 projection;

    void setPosition(glm::vec3 position);
    void setCameraDirection(glm::vec3 dir);
    void setProjection(int width, int height, float fov);
    glm::vec3 getPosition();
    glm::vec3 getDirection();
    glm::vec3 getCameraUp();
    glm::vec3 getCameraRight();

  private:
    glm::mat4 cameraTransform;
    glm::vec3 cameraPos;
    glm::vec3 cameraTarget;
    glm::vec3 cameraDirection;
    glm::vec3 up;
    glm::vec3 cameraRight;
    glm::vec3 cameraUp;
};
#endif
