#include "shader/lighting_lib.glsl"

in vec3 position;

out vec4 geoData[3];

uniform vec3 viewSpaceLightDirection;
uniform float resolution;
uniform float textureScale;

in vec2 texCoord;

in vec3 viewSpaceNormal;
in vec3 viewSpacePosition;
in vec4 shadowCoordOut;

uniform sampler2D grass;
uniform sampler2D dirt;
uniform sampler2D rock;

uniform sampler2DShadow shadowMap;


uniform sampler2D splatMap;

in vec3 norm;

void main()
{

    vec3 scene_ambient_light = vec3(0.6);
    vec3 scene_light = vec3(0.8);


    vec3 v = -normalize(viewSpacePosition);
    vec3 l= -viewSpaceLightDirection;
    vec3 n = viewSpaceNormal;

    vec2 scaledTexcoord = texCoord * resolution * textureScale;

    vec4 splat =texture(splatMap, texCoord);

    vec3 diffColor =
	splat.r * texture(grass, scaledTexcoord).xyz +
	splat.g * texture(dirt, scaledTexcoord).xyz +
	splat.b * texture(rock, scaledTexcoord).xyz;
    //diffColor = vec3(1);

    /*
    vec3 shading = phongVertex(
	diffuse
	, viewSpaceNormal,
			       viewSpaceLightDirection // minus, because its a directional light.
			       , viewSpacePosition);
    fragmentColor = vec4(shading, 1.0);
    */

    vec3 ambientMat = vec3(0.6);
    vec3 diffMat = vec3(0.8);
    float specShiny = 0;
    vec3 specColor = vec3(0);
    float diff=  calcDiff(l,n);
    float spec= calcSpec(l,n,v);
    float visibility = calcVisibility(shadowMap, diff, shadowCoordOut);

    geoData[0] = calcLighting(
	ambientMat.xyz,
	diffMat.xyz,
	specShiny,
	diffColor.xyz,
	specColor.xyz,
	diff,
	spec,
	visibility);

    geoData[1] = vec4(viewSpaceNormal,0);
    geoData[2] = vec4(viewSpacePosition, 0);
}
