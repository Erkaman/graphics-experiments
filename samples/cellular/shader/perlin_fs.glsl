
#include "shader_lib/lib.glsl"
#include "shader_lib/perlin_noise_lib.glsl"

in vec3 position;

in vec2 texCoord;

out vec4 fragmentColor;



/*
int calcLod(float dist, float maxDist, int minLod, int maxLod) {
    int lodDiff = maxLod - minLod + 1; // +1, since it's an inclusive range of [minLod, maxLod]

    float f = (maxDist / lodDiff);

    int lod = minLod + int(floor( (maxDist-dist)  /  f));

    return clamp(lod, minLod, maxLod);
}
*/

vec3 sampleDiffuseTexture() {
  vec3 color1 = vec3(0.0f, 0.5f, 0.0);
  vec3 color2 = vec3(0.0f, 0.9f, 0.0f);

  return mix(color1, color2,

    turbulence(4, vec3(texCoord.xy,0) * 1, 2, 0.5));
}

void main()
{
    fragmentColor = vec4(sampleDiffuseTexture(), 1.0);
}
