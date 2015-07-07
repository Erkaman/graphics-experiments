
#include "lib.glsl"
#include "noise_lib.glsl"

in vec3 position;

out vec4 fragmentColor;

uniform vec3 viewSpaceLightPosition;

in vec3 vertexColor;
in vec2 texCoord;

in vec3 viewSpaceNormal;
in vec3 viewSpacePosition;

#define SCALE 0.03

/*float turbulence(int octaves, vec2 P, float lacunarity, float gain)
{
  float sum = 0;
  float scale = 1;
  float totalgain = 1;
  for(int i=0;i<octaves;i++){
    sum += totalgain*snoise(P*scale);
    scale *= lacunarity;
    totalgain *= gain;
  }
  return abs(sum);
  }*/

vec3 sampleDiffuseTexture() {

//    maybe we should use the vertex position instead?



  vec3 color1 = vec3(0.0f, 0.0f, 0.0);
  vec3 color2 = vec3(0.0f, 0.6f, 0.0f);

  return mix(color1, color2,
	     vec3(0,1,0)
	     /*turbulence(8, texCoord.xy * 15, 2, 0.5) */);

//    vec3 noise = vec3(rand(  floor(texCoord.xy / SCALE ) * SCALE ) * 0.35);

//    return mix(noise, vertexColor, 0.90 );
}

void main()
{
    vec3 shading = phongVertex(sampleDiffuseTexture(), viewSpaceNormal, viewSpaceLightPosition, viewSpacePosition);

    // this is our height map color.

    fragmentColor = vec4(shading, 1.0);
}

/*
we can use less octaves for distance textures! LOD!

  float3 P = fmod(floor(p), 256.0);

  p -= floor(p);

  float3 f = fade(p);
*/
