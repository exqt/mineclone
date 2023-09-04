#include "mesh.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Mesh::Mesh() {
  count = 0;

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vboVertices);
  glGenBuffers(1, &vboUVs);
}

Mesh::~Mesh() {
  glDeleteBuffers(1, &vboVertices);
  glDeleteBuffers(1, &vboUVs);
  glDeleteVertexArrays(1, &vao);
}

void Mesh::update(std::vector<float> vertices, std::vector<float> uvs) {
  count = vertices.size() / 3;

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, vboUVs);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * uvs.size(), uvs.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glEnableVertexAttribArray(1);
}

void Mesh::bind() {
  glBindVertexArray(vao);
}

void Mesh::draw() {
  if (count == 0) return;
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, count);
}
