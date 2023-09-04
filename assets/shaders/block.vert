#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in uint aMetadata;

uniform mat4 uModel;
uniform mat4 uProjView;

out vec2 vTexCoord;
flat out uint vAmbientOcclusion;

void main() {
  vTexCoord = aTexCoord;
  vAmbientOcclusion = (aMetadata & 0xFFu);

  gl_Position = uProjView * uModel * vec4(aPosition, 1.0);
}
