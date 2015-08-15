#include "shader_lib/lib.glsl"


out vec4 fragmentColor;

uniform vec3 viewSpaceLightPosition;

//in vec3 vertexColor;
in vec2 texCoord;

in vec3 viewSpaceNormal;
in vec3 viewSpacePosition;

uniform sampler2D grass;

void main()
{
    vec4 sample = texture(grass, texCoord);
    vec3 shading = phongVertex(sample.rgb, viewSpaceNormal, viewSpaceLightPosition, viewSpacePosition);
    fragmentColor = /*vec4(viewSpaceNormal,sample.a);*/ vec4(shading.rgb, sample.a);
}
