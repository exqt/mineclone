#pragma once

#include <GL/glew.h>
#include "texture.hpp"

class Framebuffer;
using FramebufferPtr = std::shared_ptr<Framebuffer>;

class Framebuffer {

public:
  Framebuffer();
  Framebuffer(int width, int height, bool color=true, bool depth=true, bool stencil=false);
  ~Framebuffer();

  void bind();
  void unbind();
  void copyTo(FramebufferPtr other);

  void resize(int width, int height);

  void clear();

  Texture *colorTexture, *depthTexture;

private:
  GLuint fbo;
  int width, height;
};

