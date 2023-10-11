#version 330 core

in vec3 vPosition;
in vec2 vTexCoord;

out vec4 FragColor;

uniform float uTime;
uniform vec3 uCameraPosition;

// cloud
// perlin noise: https://www.shadertoy.com/view/4sfGzS
float hash( ivec3 p )    // this hash is not production ready, please
{                        // replace this by something better

    // 3D -> 1D
    int n = p.x*3 + p.y*113 + p.z*311;

    // 1D hash by Hugo Elias
  n = (n << 13) ^ n;
    n = n * (n * n * 15731 + 789221) + 1376312589;
    return float( n & ivec3(0x0fffffff))/float(0x0fffffff);
}

float noise ( in vec3 x )
{
    ivec3 i = ivec3(floor(x));
    vec3 f = fract(x);
    f = f*f*(3.0-2.0*f);

    return mix(mix(mix( hash(i+ivec3(0,0,0)),
                        hash(i+ivec3(1,0,0)),f.x),
                   mix( hash(i+ivec3(0,1,0)),
                        hash(i+ivec3(1,1,0)),f.x),f.y),
               mix(mix( hash(i+ivec3(0,0,1)),
                        hash(i+ivec3(1,0,1)),f.x),
                   mix( hash(i+ivec3(0,1,1)),
                        hash(i+ivec3(1,1,1)),f.x),f.y),f.z);
}

float fbm (vec2 p, float time) {
  float f = 0.0;
  f += 0.5000*noise(vec3(p, 0));
  f += 0.2500*noise(vec3(p*2.0, time*2.0));
  f += 0.1250*noise(vec3(p*4.0, time*4.0));
  f += 0.0625*noise(vec3(p*8.0, time*8.0));
  f /= 0.9375;
  return f;
}

float cloudHeight = 200.0;
///

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
  float cloud = 0.0;

  // cloud
  if (dir.y > 0.0) {
    vec3 pointing = vec3(sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi));
    pointing *= (cloudHeight / pointing.y);

    vec2 cloudUV = pointing.xz;
    cloud = fbm(cloudUV * 0.01, uTime*0.1);
    cloud = min(cloud, 0.7);
    cloud = pow(cloud, 1.3);

    cloud *= 1.0 - smoothstep(0.0, 2000.0, length(pointing));
  }

  float sunTheta = uTime*0.00 + pi/2. - 0.36;
  float sunPhi = 0.0;

  vec3 color = skyColorBlue;
  float sunDist = sphericalDistance(theta, phi, sunTheta, sunPhi);
  float sunIntensity = pow(clamp((0.1 - sunDist) / 0.05, 0.0, 1.0), 2.0);
  color = mix(color, skyWhite, sunIntensity);
  float horizon = pow(clamp(0.5 - abs(theta - pi/2.0), 0.0, 1.0), 2.0);
  color = mix(color, skyWhite, horizon);

  color += vec3(0.7, 0.7, 0.9) * cloud;

  FragColor = vec4(color, 1.0);
}
