#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;

out vec2 vTexCoord;

uniform vec2 uResolution;
uniform vec2 uOffset;

void main() {
  vTexCoord = aTexCoord;

  vec3 pos = (aPosition + vec3(uOffset, 0.0))/vec3(uResolution, 1.0);
  pos.xy = pos.xy * 2.0 - 1.0;
  pos.y *= -1.0;

  gl_Position = vec4(pos, 1.0);
}
