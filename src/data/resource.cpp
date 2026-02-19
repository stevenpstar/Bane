#include <bane/data/resource.hpp>
#include <bane/primitives/animatedModel.hpp>
#include <bane/primitives/model.hpp>
#include <bane/primitives/texture.hpp>
#include <bane/utility/AABB.hpp>
#include <bane/utility/shader.hpp>
#include <iostream>
#include <memory>

int GetTexture(const Resources &resources, std::string textureName) {
  if (resources.Textures.size() == 0)
    return -1;

  for (int i = 0; i < resources.Textures.size(); ++i) {
    if (resources.Textures.at(i)->name == textureName) {
      return i;
    }
  }

  return -1;
}

std::string GetTextureName(std::unique_ptr<Resources> &resources, unsigned int textureID) {
  if (resources->Textures.size() == 0)
    return "";

  for (int i = 0; i < resources->Textures.size(); ++i) {
    if (resources->Textures.at(i)->id == textureID) {
      return resources->Textures.at(i)->name;
    }
  }

  return "";
}

int GetShader(const std::unique_ptr<Resources> &resources, std::string shaderName) {
  if (resources->Shaders.size() == 0) {
    std::cout << "No Shaders found in resources\n";
    return -1;
  }

  for (int i = 0; i < resources->Shaders.size(); ++i) {
    if (resources->Shaders.at(i)->name == shaderName) {
      return i;
    }
  }

  std::cout << "Shader not found: " << shaderName << "\n";
  return -1;
}

int GetModel(const Resources &resources, std::string modelName) {
  if (resources.Models.size() == 0)
    return -1;

  for (int i = 0; i < resources.Models.size(); ++i) {
    if (resources.Models.at(i)->name == modelName) {
      return i;
    }
  }

  return -1;
}

int GetCollider(const Resources &resources, std::string colliderName) {
  if (resources.Colliders.size() == 0)
    return -1;

  for (int i = 0; i < resources.Colliders.size(); ++i) {
    if (resources.Colliders.at(i)->name == colliderName) {
      return i;
    }
  }

  return -1;
}

int GetAnimatedModel(const Resources &resources, std::string modelName) {
  if (resources.Models.size() == 0)
    return -1;

  for (int i = 0; i < resources.AnimatedModel.size(); ++i) {
    if (resources.AnimatedModel.at(i)->name == modelName) {
      return i;
    }
  }

  return -1;
}
