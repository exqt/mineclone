#version 330 core

in vec3 vPosition;
in vec2 vTexCoord;

out vec4 FragColor;
uniform sampler2D tex;

void main() {
  vec2 uv = vTexCoord;

  vec4 color = texture(tex, uv);
  color = vec4(1.0, 1.0, 1.0, 1.0);

  FragColor = color;
}
