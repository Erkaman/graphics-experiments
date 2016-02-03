#include "shader/lighting_lib.glsl"

in vec3 position;

out vec4 geoData[3];

uniform vec3 viewSpaceLightDirection;
uniform float resolution;
uniform float textureScale;
uniform float aoOnly;

in vec2 texCoord;

in vec3 viewSpaceNormal;
in vec3 viewSpacePosition;
in vec4 shadowCoordOut;

uniform sampler2D grass;
uniform sampler2D dirt;
uniform sampler2D rock;

uniform sampler2DShadow shadowMap;


uniform sampler2D splatMap;
uniform sampler2D aoMap;

in vec3 outn;


uniform vec3 ambientLight;
uniform vec3 sceneLight;


void main()
{
    vec3 v = -normalize(viewSpacePosition);
    vec3 l= -viewSpaceLightDirection;
    vec3 n = viewSpaceNormal;

    vec2 scaledTexcoord = texCoord * resolution * textureScale;

    vec4 splat =texture(splatMap, texCoord);

    vec3 diffColor =
	splat.r * texture(grass, scaledTexcoord).xyz +
	splat.g * texture(dirt, scaledTexcoord).xyz +
	splat.b * texture(rock, scaledTexcoord).xyz;

    float specShiny = 0;
    vec3 specColor = vec3(0);
    float diff=  calcDiff(l,n);
    float spec= calcSpec(l,n,v);

#ifdef SHADOW_MAPPING
    float visibility = calcVisibility(shadowMap, diff, shadowCoordOut);
#else
    float visibility = 1.0;
#endif

    geoData[0] =vec4(vec3(1.0-texture(aoMap, texCoord).r), 1.0) * aoOnly +
	(1.0 - aoOnly)*calcLighting(
	ambientLight* (1.0 -texture(aoMap, texCoord).r),
	sceneLight,
	specShiny,
	diffColor.xyz,
	specColor.xyz,
	diff,
	spec,
	visibility,
	vec3(0) );

    geoData[1] = vec4(viewSpaceNormal,0);
    geoData[2] = vec4(viewSpacePosition, 0);
}
