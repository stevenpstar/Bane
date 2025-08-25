#ifndef BANE_SCENE
#define BANE_SCENE
#include <bane/primitives/pointLight.hpp>
#include <bane/scene/sceneModel.hpp>
#include <glm/glm.hpp>
#include <vector>
struct SceneAnimatedModel;
struct SceneCharacter;
struct Scene {
  std::string sceneName;
  // maybe at some point a 2d vector, so we can instance static mesh rendering automatically(?)
  std::vector<SceneModel> staticModels;
  // std::vector<SceneAnimatedModel> animatedModels;
  // std::vector<SceneCharacter> characters;
  // increase point light count eventually
  // std::array<PointLight, 2> pointLights;
  glm::vec3 lightDirection;
  float ambientStrength;
};
#endif
