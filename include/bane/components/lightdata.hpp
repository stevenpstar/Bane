#ifndef BANE_LIGHT_DATA
#define BANE_LIGHT_DATA
#include <bane/primitives/pointLight.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <vector>
struct LightData {
  glm::vec3 dirLight;
  glm::mat4 lightSpaceMatrix;
  float ambientStrength;
  std::vector<std::unique_ptr<PointLight>> pointLights;
};
#endif
