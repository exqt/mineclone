#include "shader.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(std::string vertexPath, std::string fragmentPath) {
  std::ifstream vertexFile(vertexPath);
  std::ifstream fragmentFile(fragmentPath);
  std::stringstream vertexStream;
  std::stringstream fragmentStream;

  vertexStream << vertexFile.rdbuf();
  fragmentStream << fragmentFile.rdbuf();
  vertexFile.close();
  fragmentFile.close();

  std::string vertexCode = vertexStream.str();
  std::string fragmentCode = fragmentStream.str();

  const char* vertexSource = vertexCode.c_str();
  const char* fragmentSource = fragmentCode.c_str();

  int success;
  char infoLog[512];

  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vertexSource, NULL);
  glCompileShader(vertex);
  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(vertex, 512, NULL, infoLog);
    std::cerr << vertexPath << std::endl;
    std::cerr << "Vertex shader compilation failed:\n" << infoLog << std::endl;
    std::exit(-1);
  }

  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fragmentSource, NULL);
  glCompileShader(fragment);
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(fragment, 512, NULL, infoLog);
    std::cerr << fragmentPath << std::endl;
    std::cerr << "Fragment shader compilation failed:\n" << infoLog << std::endl;
    std::exit(-1);
  }

  id = glCreateProgram();
  glAttachShader(id, vertex);
  glAttachShader(id, fragment);
  glLinkProgram(id);
  glGetProgramiv(id, GL_LINK_STATUS, &success);

  if (!success) {
    glGetProgramInfoLog(id, 512, NULL, infoLog);
    std::cerr << vertexPath << std::endl;
    std::cerr << "Shader program linking failed:\n" << infoLog << std::endl;
    std::exit(-1);
  }
}

Shader::~Shader() {
  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

void Shader::bind() {
  glUseProgram(id);
}

void Shader::setVec4(std::string name, glm::vec4 vector) {
  glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(vector));
}

void Shader::setVec3(std::string name, glm::vec3 vector) {
  glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(vector));
}

void Shader::setVec2(std::string name, glm::vec2 vector) {
  glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(vector));
}

void Shader::setMat4(std::string name, glm::mat4 matrix) {
  auto loc = glGetUniformLocation(id, name.c_str());
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setInt(std::string name, int value) {
  glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setFloat(std::string name, float value) {
  glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}
