#include "bane/components/bobject.hpp"
#include <bane/components/camera.hpp>
#include <bane/components/character.hpp>
#include <bane/primitives/animatedModel.hpp>
#include <bane/utility/shader.hpp>
#include <glm/ext/vector_float3.hpp>
Character::Character(const char *modelPath, Shader *shader) {
  // AnimatedModel model = AnimatedModel(modelPath, glm::vec3(0.f, 0.f, 0.f));
  this->shader = shader;
  randomColour = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
}

void Character::Render(Camera *camera) {}

void Character::Update(float deltaTime) {
  // update collision box position
}
