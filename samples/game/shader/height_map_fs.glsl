#include "shader_lib/lib.glsl"

in vec3 position;

out vec4 fragmentColor;

uniform vec3 viewSpaceLightPosition;

in vec3 vertexColor;
in vec2 texCoord;

in vec3 viewSpaceNormal;
in vec3 viewSpacePosition;

#define SCALE 0.03

vec3 sampleDiffuseTexture() {

    return vec3(1,0,0);
}

void main()
{
    vec3 shading = phongVertex(sampleDiffuseTexture(), viewSpaceNormal, viewSpaceLightPosition, viewSpacePosition);

    // this is our height map color.

    fragmentColor = vec4(shading, 1.0);
}
