#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in uint aMetadata;

uniform mat4 uProjView;
uniform vec3 uOffset;

out vec2 vTexCoord;
flat out uint vAmbientOcclusion;

void main() {
  vTexCoord = aTexCoord;
  vAmbientOcclusion = (aMetadata & 0xFFu);

  gl_Position = uProjView * vec4(aPosition + uOffset, 1.0);
}
