
#include "lib.glsl"
#include "noise_lib.glsl"

out vec4 fragmentColor;

uniform vec3 viewSpaceLightPosition;

in vec3 vertexColor;
in vec2 texCoord;

in vec3 viewSpaceNormal;
in vec3 viewSpacePosition;

#define SCALE 0.03

vec3 sampleDiffuseTexture() {

    vec3 noise = vec3(rand(  floor(texCoord.xy / SCALE ) * SCALE ) * 0.35);

    return mix(noise, vertexColor, 0.90 );
}

void main()
{
    vec3 shading = phongVertex(sampleDiffuseTexture(), viewSpaceNormal, viewSpaceLightPosition, viewSpacePosition);

    // this is our height map color.

    fragmentColor = vec4(shading, 1.0);
}
