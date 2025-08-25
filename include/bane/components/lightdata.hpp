#ifndef BANE_LIGHT_DATA
#define BANE_LIGHT_DATA
#include <glm/glm.hpp>
struct LightData {
  glm::vec3 dirLight;
  float ambientStrength;
};
#endif
