#version 330 core

in vec3 vPosition;
in vec2 vTexCoord;

out vec4 FragColor;

uniform float uTime;

vec3 skyColorBlue = vec3(0.5, 0.6, 0.8);
vec3 skyColorRed = vec3(0.8, 0.6, 0.5);
vec3 skyWhite = vec3(1.0, 1.0, 1.0);
float pi = 3.1415926535897932384626433832795;

// https://en.wikipedia.org/wiki/Spherical_coordinate_system#Distance_in_spherical_coordinates
float sphericalDistance(float theta0, float phi0, float theta1, float phi1) {
  return sqrt(2.0 - 2.0 * (sin(theta0) * sin(theta1) * cos(phi0 - phi1) + cos(theta0) * cos(theta1)));
}

void main() {
  // vec2 uv = vTexCoord;
  // vec4 color = texture(tex, uv);
  vec3 dir = normalize(vPosition);

  float theta = acos(dir.y);
  float phi = atan(dir.x, dir.z);

  float sunTheta = uTime*0.00 + pi/2 - 0.36;
  float sunPhi = 0.0;

  vec3 color = skyColorBlue;
  float sunDist = sphericalDistance(theta, phi, sunTheta, sunPhi);
  float sunIntensity = pow(clamp((0.1 - sunDist) / 0.05, 0.0, 1.0), 2.0);
  color = mix(color, skyWhite, sunIntensity);
  float horizon = pow(clamp(0.5 - abs(theta - pi/2.0), 0.0, 1.0), 2.0);
  color = mix(color, skyWhite, horizon);

  FragColor = vec4(color, 1.0);
}
