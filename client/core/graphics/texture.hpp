#pragma once

#include <GL/glew.h>

#include <string>
#include <memory>

class Texture {
public:
  Texture(int width, int height, GLenum format, void* data);
  Texture(std::string path);
  ~Texture();

  void bind(int unit=0);

  int width, height;
  GLenum format;
  GLuint id;

private:
  void load(void* data);
};

using TexturePtr = std::shared_ptr<Texture>;
