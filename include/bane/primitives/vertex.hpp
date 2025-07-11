#ifndef BANE_VERTEX
#define BANE_VERTEX
#define MAX_BONE_INFLUENCE 4
#include <glm/glm.hpp>
struct Vertex {
  glm::vec3 Position;
  glm::vec3 Normal;
  glm::vec2 TexCoords;

  int BoneIds[MAX_BONE_INFLUENCE];
  float Weights[MAX_BONE_INFLUENCE];
};
#endif
