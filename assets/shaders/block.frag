#version 330 core

in vec2 vTexCoord;
flat in uint vAmbientOcclusion;

out vec4 FragColor;

uniform sampler2D tex;

// 00 01
// 10 11
float bilinear(float v00, float v01, float v10, float v11, vec2 uv) {
  float v0 = mix(v00, v01, uv.x);
  float v1 = mix(v10, v11, uv.x);
  return mix(v0, v1, uv.y);
}

void main() {
  vec2 aoXY = fract(vTexCoord);
  vec2 uv = vTexCoord / 256 * 16;
  vec4 color = texture(tex, uv);

  float a00 = (vAmbientOcclusion >> 0) & 0x3u;
  float a01 = (vAmbientOcclusion >> 2) & 0x3u;
  float a10 = (vAmbientOcclusion >> 4) & 0x3u;
  float a11 = (vAmbientOcclusion >> 6) & 0x3u;
  float ao = 1.0 - bilinear(a00, a01, a10, a11, aoXY) / 6.0;

  color.xyz *= ao;
  // color = vec4(vec3(ao), 1.0);

  FragColor = color;
}
