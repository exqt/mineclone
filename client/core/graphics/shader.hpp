#pragma once

#include <string>
#include <memory>
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
  Shader(std::string vertexPath, std::string fragmentPath);
  ~Shader();

  void bind();

  void setVec4(std::string name, glm::vec4 vector);
  void setVec3(std::string name, glm::vec3 vector);
  void setMat4(std::string name, glm::mat4 matrix);
  void setInt(std::string name, int value);
  void setFloat(std::string name, float value);

private:
  Shader();

  GLuint vertex, fragment;
  GLuint id;
};

using ShaderPtr = std::shared_ptr<Shader>;
