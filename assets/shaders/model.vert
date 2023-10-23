#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 uModel;
uniform mat4 uProjView;

out vec3 vPosition;
out vec2 vTexCoord;

void main() {
  vTexCoord = aTexCoord;
  vPosition = aPosition;

  vec4 pos = vec4(aPosition, 1.0);

  gl_Position = uProjView * uModel * pos;
}
