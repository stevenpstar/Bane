#include <bane/primitives/primitives.hpp>
std::array<float, 18>& createColouredTriangle(std::array<float, 18>& vert)
{
  vert = {
    // positions       // colours
    0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
   -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f
  };

  return vert;
}

