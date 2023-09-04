#pragma once

#include <GL/glew.h>
#include <vector>
#include <initializer_list>
#include <memory>
#include "texture.hpp"

class Mesh {
public:
  Mesh();
  ~Mesh();

  void update(std::vector<float> vertices, std::vector<float> uvs);

  void bind();

  void draw();

  GLuint count;
  GLuint vao;
  GLuint vboVertices, vboUVs;
};

using MeshPtr = std::shared_ptr<Mesh>;
