#include "shader_lib/lib.glsl"

in vec3 position;

out vec4 fragmentColor;

uniform vec3 viewSpaceLightDirection;
uniform float resolution;

//in vec3 vertexColor;
in vec2 texCoord;

in vec3 viewSpaceNormal;
in vec3 viewSpacePosition;

uniform sampler2D grass;
uniform sampler2D dirt;
uniform sampler2D rock;

uniform sampler2D splatMap;

in float id;

in vec3 norm;

#define SCALE 0.03

/*
vec3 sampleDiffuseTexture() {

    float y = (position.y - (-4.900633)) / (-2.410543 - (-4.900633) );

// min max -4.900633, -2.410543

    float l;

    if(y > 0.75) {


	if(y > 0.75 && y < 0.85) {
	    l = 1 - ((y - 0.75) * 10);
	} else if(y > 0.85) {
	    l = 0;
	} else {
	    l = 1;
	}

	return mix(texture(snow,texCoord).xyz, texture(grass,texCoord).xyz, l);

    } else {

	if(y > 0.30 && y < 0.35) {
	    l = 1 - ((y - 0.30) * 20);
	} else if(y > 0.35) {
	    l = 0;
	} else {
	    l = 1;
	}
	return mix(texture(grass,texCoord).xyz, texture(sand,texCoord).xyz, l);

    }
//
}
*/

void main()
{
    vec2 scaledTexcoord = texCoord * resolution;

    vec4 splat =texture(splatMap, texCoord);



    vec3 diffuse =
	splat.r * texture(grass, scaledTexcoord).xyz +
	splat.g * texture(dirt, scaledTexcoord).xyz +
	splat.b * texture(rock, scaledTexcoord).xyz;



    vec3 shading = phongVertex(
	diffuse
	/*sampleDiffuseTexture()*/, viewSpaceNormal,
			       viewSpaceLightDirection // minus, because its a directional light.
			       , viewSpacePosition);
    fragmentColor = vec4(shading, 1.0);
//    fragmentColor = vec4(vec3(height), 1.0);

}
