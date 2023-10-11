#version 330 core

in vec3 vPosition;
in vec2 vTexCoord;

out vec4 FragColor;
uniform sampler2D tex;

void main() {
  vec2 uv = vTexCoord;

  vec4 color = vec4(0.04, 0.12, 0.39, 1.0);
  FragColor = color;
}
