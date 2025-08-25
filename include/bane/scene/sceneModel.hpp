#ifndef BANE_SCENE_MODEL
#define BANE_SCENE_MODEL
#include <glm/glm.hpp>
#include <string>
struct SceneModel {
  int id;
  std::string name;
  std::string filePath;
  glm::vec3 position;
  // add rotation prob
};
#endif
