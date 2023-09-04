#include "lines.hpp"

Lines::Lines() {
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vboVertices);
  glGenBuffers(1, &vboColors);
}

Lines::~Lines() {
  glDeleteBuffers(1, &vboVertices);
  glDeleteBuffers(1, &vboColors);
  glDeleteVertexArrays(1, &vao);
}

void Lines::update(std::vector<float> vertices, std::vector<float> colors) {
  count = vertices.size() / 3;

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, vboColors);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * colors.size(), colors.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glEnableVertexAttribArray(1);
}

void Lines::bind() {
  glBindVertexArray(vao);
}

void Lines::draw() {
  if (count == 0) return;
  glBindVertexArray(vao);
  glDrawArrays(GL_LINES, 0, count);
}
