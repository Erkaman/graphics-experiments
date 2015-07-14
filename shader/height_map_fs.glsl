
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

uniform vec3 cameraPos;

int calcLod(float dist, float maxDist, int minLod, int maxLod) {
    int lodDiff = maxLod - minLod + 1; // +1, since it's an inclusive range of [minLod, maxLod]

    float f = (maxDist / lodDiff);

    int lod = minLod + int(floor( (maxDist-dist)  /  f));

    return clamp(lod, minLod, maxLod);
}

vec3 sampleDiffuseTexture() {

//    maybe we should use the vertex position instead?



/*  vec3 color1 = vec3(0.4f, 0.7f, 0.0);
  vec3 color2 = vec3(0.0f, 1.0f, 0.0f);
*/

  vec3 color1 = vec3(0.0f, 0.5f, 0.0);
  vec3 color2 = vec3(0.0f, 0.9f, 0.0f);

  vec3 diff = cameraPos.xyz - position.xyz;

  float dist = sqrt(dot(diff,diff));

  int lod = calcLod(dist, 100, 1, 4 );
/*
  if(lod == 1) {
      return vec3(0.5,0.5,0.5); // gray
  }else if(lod == 2) {
      return vec3(1,0,0); // red
  } else if(lod == 3) {
      return vec3(0,1,0); // green.
  }else if(lod == 4) {
      return vec3(0,0,1); // blue
  }else if(lod == 5) {
      return vec3(0,0,0); // black
  }else if(lod == 6) {
      return vec3(1,1,0); // yellow
  }else if(lod == 7) {
      return vec3(0,1,1); // cyan(light blue)
  } else {
      return vec3(1,0,1); //magenta(light purple)
      }
*/
  return mix(color1, color2,

    turbulence(lod, vec3(texCoord.xy,0) * 1, 2, 0.5));
}

void main()
{
    vec3 shading = phongVertex(sampleDiffuseTexture(), viewSpaceNormal, viewSpaceLightPosition, viewSpacePosition);

    // this is our height map color.

    fragmentColor = vec4(shading, 1.0);
}
