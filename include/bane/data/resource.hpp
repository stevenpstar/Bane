#ifndef BANE_RENDER_DATA
#define BANE_RENDER_DATA
#include "bane/components/bobject.hpp"
#include <memory>
#include <string>
#include <vector>

class Shader;
class Model;
class AABB;
class AnimatedModel;
struct Texture;

struct Resources {
  std::vector<std::unique_ptr<Texture>> Textures;
  std::vector<std::unique_ptr<Shader>> Shaders;
  std::vector<std::unique_ptr<AABB>> Colliders;
  std::vector<std::unique_ptr<Bobject>> Models;
  //  std::vector<std::unique_ptr<AnimatedModel>> AnimatedModel;
};

int GetTexture(Resources &resources, std::string textureName);
int GetCollider(Resources &resources, std::string collider);
int GetShader(Resources *resources, std::string shaderName);
int GetModel(Resources &resources, std::string modelName);
// int GetAnimatedModel(Resources &resources, std::string textureName);

#endif
