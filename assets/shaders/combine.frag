#version 330 core

in vec3 vPosition;
in vec2 vTexCoord;

uniform sampler2D uMainColor;
uniform sampler2D uMainDepth;
uniform sampler2D uWaterColor;
uniform sampler2D uWaterDepth;
uniform sampler2D uSkyColor;

uniform float uCameraNear;
uniform float uCameraFar;

out vec4 FragColor;

float fogNear = 50.0;
float fogFar = 100.0;

float LinearizeDepth(float depth) {
  float z = depth * 2.0 - 1.0; // back to NDC
  return (2.0 * uCameraNear * uCameraFar) / (uCameraFar + uCameraNear - z * (uCameraFar - uCameraNear));
}

void main() {
  vec2 uv = vTexCoord;

  vec3 mainColor = texture(uMainColor, uv).rgb;
  float mainDepth = texture(uMainDepth, uv).r;
  vec3 waterColor = texture(uWaterColor, uv).rgb;
  float waterDepth = texture(uWaterDepth, uv).r;
  vec3 skyColor = texture(uSkyColor, uv).rgb;

  mainDepth = LinearizeDepth(mainDepth);
  waterDepth = LinearizeDepth(waterDepth);

  vec3 color = skyColor;

  if (mainDepth < uCameraFar * 0.99) {
    color = mainColor;
  }

  if (waterDepth < mainDepth && mainDepth < uCameraFar * 0.99) {
    float depthDiff = mainDepth - waterDepth;
    color = mix(mainColor, waterColor, 0.5 + depthDiff * 0.01);
  }

  FragColor = vec4(color, 1.0);
}
