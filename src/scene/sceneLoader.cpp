#include "bane/scene/sceneModel.hpp"
#include "glm/fwd.hpp"
#include <bane/core/scene.hpp>
#include <bane/scene/sceneLoader.hpp>
#include <exception>
#include <fstream>
#include <iostream>
#include <json.hpp>
using json = nlohmann::json;

Scene loadScene(std::string scenePath) {
  Scene loadedScene;
  loadedScene.lightDirection = glm::vec3(0.f, 0.f, 0.f);
  std::ifstream file(scenePath);
  file.seekg(0);
  json sceneData;
  try {

    sceneData = json::parse(std::ifstream(scenePath));
  } catch (json::parse_error &err) {
    std::cout << "Failed to parse?" << err.what() << "\n";
    return loadedScene;
  }
  // std::cout << "Parsing error maybe\n";
  if (sceneData.contains("sceneName"))
    loadedScene.sceneName = sceneData["sceneName"];
  if (sceneData.contains("lightDirection_x"))
    loadedScene.lightDirection.x = sceneData["lightDirection_x"];
  if (sceneData.contains("lightDirection_y"))
    loadedScene.lightDirection.y = sceneData["lightDirection_y"];
  if (sceneData.contains("lightDirection_z"))
    loadedScene.lightDirection.z = sceneData["lightDirection_z"];

  //  std::cout << "Found name!\n";
  for (int i = 0; i < sceneData["bobjects"].size(); ++i) {
    // std::cout << "Loading model data\n";
    SceneModel sceneModel;
    sceneModel.id = sceneData["bobjects"].at(i)["id"];
    // std::cout << "id loaded? : " << sceneModel.id << "\n";
    sceneModel.name = sceneData["bobjects"].at(i)["name"];
    sceneModel.filePath = sceneData["bobjects"].at(i)["filePath"];
    //  std::cout << "Loaded name and path\n";
    sceneModel.position = glm::vec3(0.f, 0.f, 0.f);
    sceneModel.position.x = sceneData["bobjects"].at(i)["xpos"];
    sceneModel.position.y = sceneData["bobjects"].at(i)["ypos"];
    sceneModel.position.z = sceneData["bobjects"].at(i)["zpos"];
    //   std::cout << "model: " << sceneModel.name << "\n";
    //   std::cout << "xpos: " << sceneModel.position.x << "\n";
    //   std::cout << "ypos: " << sceneModel.position.y << "\n";
    //   std::cout << "zpos: " << sceneModel.position.z << "\n";
    loadedScene.staticModels.push_back(sceneModel);
  }
  //  std::cout << "Did we load the scene?\n";
  //  std::cout << std::setw(4) << sceneData << '\n';
  return loadedScene;
}

Scene createEmptyScene(std::string name) {
  Scene newScene;
  newScene.sceneName = name;
  return newScene;
}
