#include "shader_lib/lib.glsl"

in vec3 position;

uniform vec3 viewSpaceLightPosition;
in vec3 viewSpaceNormal;
in vec3 viewSpacePosition;


out vec4 fragmentColor;

in vec2 texCoord;

uniform sampler2D tex;

// it's as if tex is not loaded

void main(){

    vec4 sample = texture(tex,texCoord);
    if(sample.a < 0.75)
	discard;

    vec3 shading = phongVertex(sample.rgb, viewSpaceNormal, viewSpaceLightPosition, viewSpacePosition);
    fragmentColor = vec4(shading, 1.0);
//    fragmentColor = vec4(texCoord,0,1);

}
