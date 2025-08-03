#ifndef BANE_TEXTURE
#define BANE_TEXTURE
#include <string>
struct Texture {
  unsigned int id;
  std::string type;
  std::string path;
};

unsigned int createTexture(const char* path);
unsigned int createSkyBoxTexture(const char* folder);

#endif
