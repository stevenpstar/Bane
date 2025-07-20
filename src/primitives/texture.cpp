#include <glad/glad.h>
#include <iostream>
#include <stbimage/stb_image.h>

unsigned int createTexture(const char* path)
{
  unsigned int texture;
  int width, height, nrChannels;
  unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
  if (!data)
  {
    std::cout << "Error loading image. "<<std::endl;
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
  if (data)
  {
    if (nrChannels == 1)
    {
      format = GL_RED;
    } else if (nrChannels == 3)
    {
      format = GL_RGB;
    } else if (nrChannels == 4)
    {
      format = GL_RGBA;
    }
  } else {
    std::cout << "data error for image texture\n";
  }
  glTexImage2D(GL_TEXTURE_2D, 0,
      format,
      width,
      height,
      0,
      format,
      GL_UNSIGNED_BYTE,
      data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);
  return texture;
}
