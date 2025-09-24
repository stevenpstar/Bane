#include "bane/scene/sceneModel.hpp"
#include "glm/fwd.hpp"
#include <bane/core/scene.hpp>
#include <bane/scene/sceneLoader.hpp>
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
  if (sceneData.contains("ambientStrength"))
    loadedScene.ambientStrength = sceneData["ambientStrength"];

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

    sceneModel.rotation = glm::vec3(0.f, 0.f, 0.f);
    if (sceneData["bobjects"].at(i).contains("xrot"))
      sceneModel.rotation.x = sceneData["bobjects"].at(i)["xrot"];
    if (sceneData["bobjects"].at(i).contains("yrot"))
      sceneModel.rotation.y = sceneData["bobjects"].at(i)["yrot"];
    if (sceneData["bobjects"].at(i).contains("zrot"))
      sceneModel.rotation.z = sceneData["bobjects"].at(i)["zrot"];

    sceneModel.scale = glm::vec3(0.f, 0.f, 0.f);
    if (sceneData["bobjects"].at(i).contains("xscale"))
      sceneModel.scale.x = sceneData["bobjects"].at(i)["xscale"];
    if (sceneData["bobjects"].at(i).contains("yscale"))
      sceneModel.scale.y = sceneData["bobjects"].at(i)["yscale"];
    if (sceneData["bobjects"].at(i).contains("zscale"))
      sceneModel.scale.z = sceneData["bobjects"].at(i)["zscale"];

    if (sceneData["bobjects"].at(i).contains("diffuse")) {
      sceneModel.diffuse = sceneData["bobjects"].at(i)["diffuse"];
    } else {
      std::cout << "Could not load texture\n";
    }
    loadedScene.staticModels.push_back(sceneModel);
  }

  if (sceneData.contains("pointlights")) {
    for (int i = 0; i < sceneData["pointlights"].size(); ++i) {
      // load point light data here
      PointLight light;
      light.position = glm::vec3(0.f);
      if (sceneData["pointlights"].at(i).contains("xpos"))
        light.position.x = sceneData["pointlights"].at(i)["xpos"];
      if (sceneData["pointlights"].at(i).contains("ypos"))
        light.position.y = sceneData["pointlights"].at(i)["ypos"];
      if (sceneData["pointlights"].at(i).contains("zpos"))
        light.position.z = sceneData["pointlights"].at(i)["zpos"];

      light.diffuse = glm::vec3(1.f);
      if (sceneData["pointlights"].at(i).contains("r"))
        light.diffuse.r = sceneData["pointlights"].at(i)["r"];
      if (sceneData["pointlights"].at(i).contains("g"))
        light.diffuse.g = sceneData["pointlights"].at(i)["g"];
      if (sceneData["pointlights"].at(i).contains("b"))
        light.diffuse.b = sceneData["pointlights"].at(i)["b"];

      light.intensity = 10.f;

      if (sceneData["pointlights"].at(i).contains("intensity"))
        light.intensity = sceneData["pointlights"].at(i)["intensity"];
      // TODO: These should also be configurable
      light.constant = 1.f;
      light.linear = 0.09f;
      light.quadratic = 0.032f;
      loadedScene.pointLights.push_back(light);
    }
  }

  if (sceneData.contains("colliders")) {
    for (int i = 0; i < sceneData["colliders"].size(); ++i) {
      glm::vec3 origin = glm::vec3(0.f);
      float xdim = 1.f;
      float ydim = 1.f;
      float zdim = 1.f;
      std::string name = "";
      if (sceneData["colliders"].at(i).contains("originx"))
        origin.x = sceneData["colliders"].at(i)["originx"];
      if (sceneData["colliders"].at(i).contains("originy"))
        origin.y = sceneData["colliders"].at(i)["originy"];
      if (sceneData["colliders"].at(i).contains("originz"))
        origin.z = sceneData["colliders"].at(i)["originz"];
      // dim
      if (sceneData["colliders"].at(i).contains("xdim"))
        xdim = sceneData["colliders"].at(i)["xdim"];
      if (sceneData["colliders"].at(i).contains("ydim"))
        ydim = sceneData["colliders"].at(i)["ydim"];
      if (sceneData["colliders"].at(i).contains("zdim"))
        zdim = sceneData["colliders"].at(i)["zdim"];

      if (sceneData["colliders"].at(i).contains("name"))
        name = sceneData["colliders"].at(i)["name"];
      AABB aabb = AABB(origin, xdim, ydim, zdim, nullptr, name);
      loadedScene.colliders.push_back(aabb);
    }
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
