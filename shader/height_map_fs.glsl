
#include "phong.glsl"

out vec4 fragmentColor;

uniform vec3 viewSpaceLightPosition;

uniform sampler2D noiseSampler;

in vec3 vertexColor;
in vec2 texCoord;

in vec3 viewSpaceNormal;
in vec3 viewSpacePosition;

vec3 sampleDiffuseTexture() {
    return
	mix(texture(noiseSampler, texCoord).xyz, vertexColor, 0.90 );
//	vertexColor;
}


void main()
{
    vec3 shading = phongVertex(sampleDiffuseTexture(), viewSpaceNormal, viewSpaceLightPosition, viewSpacePosition);

    // this is our height map color.

    fragmentColor = vec4(shading, 1.0);
}
