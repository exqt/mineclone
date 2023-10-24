#version 330 core

in vec2 vTexCoord;

out vec4 FragColor;

uniform sampler2D tex;
uniform vec2 uTextureSize;

void main() {
  vec2 uv = vTexCoord / uTextureSize;
  vec4 color = texture(tex, uv);
  if (color.a < 0.1) {
    discard;
  }

  FragColor = color;
}
