#pragma once

#include <GL/glew.h>
#include <vector>
#include <initializer_list>
#include "texture.hpp"

class Lines {
public:
  Lines();
  ~Lines();

  void update(std::vector<float> vertices, std::vector<float> color);

  void bind();

  void draw();

  GLuint count;
  GLuint vao;
  GLuint vboVertices, vboColors;
};

