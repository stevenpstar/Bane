#include "bane/primitives/texture.hpp"
#include <glad/glad.h>
#include <iostream>
#include <stbimage/stb_image.h>
#include <vector>

unsigned int createTexture(const char *path) {
  unsigned int texture;
  int width, height, nrChannels;
  unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
  if (!data) {
    std::cout << "Error loading image. " << std::endl;
  } else {
    std::cout << "Loaded image!\n";
  }

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  GLenum format;
  GLenum format2;
  if (data) {
    if (nrChannels == 1) {
      format = GL_RED;
      format2 = GL_RED;
    } else if (nrChannels == 3) {
      format = GL_SRGB;
      format2 = GL_RGB;
    } else if (nrChannels == 4) {
      format = GL_SRGB_ALPHA;
      format2 = GL_RGBA;
    }
  } else {
    std::cout << "data error for image texture\n";
  }
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format2, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);
  return texture;
}

unsigned int createSkyBoxTexture(const char *folder) {
  std::vector<std::string> cubeFaces = {"right.png", "left.png", "top.png", "bottom.png", "front.png", "back.png"};

  unsigned int textureId;
  glGenTextures(1, &textureId);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

  int width, height, nrChannels;
  for (unsigned int i = 0; i < cubeFaces.size(); ++i) {
    std::string filePath = folder + cubeFaces[i];
    unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
      stbi_image_free(data);
    } else {
      std::cout << "Cubemap texture failed to load at path: " << cubeFaces[i] << "\n";
      stbi_image_free(data);
    }
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  return textureId;
}
