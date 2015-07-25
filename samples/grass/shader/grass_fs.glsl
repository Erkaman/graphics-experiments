#include "shader_lib/lib.glsl"

in vec3 position;
in vec3 normal;

out vec4 fragmentColor;

uniform vec3 viewSpaceLightPosition;

//in vec3 vertexColor;
in vec2 texCoord;

in vec3 viewSpaceNormal;
in vec3 viewSpacePosition;

vec3 sampleDiffuseTexture() {
    return vec3(0,1,0);
}

void main()
{
    vec3 shading = phongVertex(sampleDiffuseTexture(), viewSpaceNormal, viewSpaceLightPosition, viewSpacePosition);
    fragmentColor = vec4(shading, 1.0);
}
