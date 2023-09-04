#include "texture.hpp"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(int width, int height, GLenum format, void* data) {
  this->width = width;
  this->height = height;
  this->format = format;
  load(data);
}

Texture::Texture(std::string path) {
  int nChannels;
  unsigned char* data = stbi_load(path.c_str(), &width, &height, &nChannels, 0);

  if (nChannels == 1) {
    format = GL_RED;
  } else if (nChannels == 2) {
    format = GL_RG;
  } else if (nChannels == 3) {
    format = GL_RGB;
  } else if (nChannels == 4) {
    format = GL_RGBA;
  }

  if (data == nullptr) {
    std::cerr << "Could not load image " << path << std::endl;
    std::exit(-1);
  }

  load(data);
  stbi_image_free(data);
}

Texture::~Texture() {
  glDeleteTextures(1, &id);
}

void Texture::load(void* data) {
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   // Set texture wrapping to GL_REPEAT (usually basic wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // Set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  if (format == GL_DEPTH_COMPONENT) {
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_FLOAT, data);
  }
  else {
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
  }

  glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::bind(int unit) {
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_2D, id);
}
