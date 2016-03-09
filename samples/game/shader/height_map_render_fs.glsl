#include "shader/lighting_lib.glsl"

in vec3 position;

out vec4 geoData[3];

uniform vec3 viewSpaceLightDirection;
uniform float aoOnly;

in vec2 texCoord;

in vec3 viewSpaceNormal;
in vec3 viewSpacePosition;

uniform sampler2D grass;
uniform sampler2D dirt;
uniform sampler2D rock;
uniform sampler2D road;

uniform sampler2DShadow shadowMap;

uniform sampler2D splatMap;
uniform sampler2D aoMap;

in vec3 outn;


uniform vec3 ambientLight;
uniform vec3 sceneLight;

void main()
{

    // TODO OPTIMIZE.
    vec2 scaledTexcoord = texCoord * resolution * textureScale;


    vec4 splat =texture(splatMap, texCoord);

    vec3 diffColor =
	splat.r * texture(grass, scaledTexcoord).xyz +
	splat.g * texture(dirt, scaledTexcoord).xyz +
	splat.b * texture(rock, scaledTexcoord).xyz;

    // TODO, use mix function here!
    diffColor = (splat.a) * texture(road, scaledTexcoord).xyz + (1.0 - splat.a) * diffColor;


   // shadowing is done in screenspace, so comment out.
/*
#ifdef SHADOW_MAPPING
    float visibility = calcVisibility(shadowMap, diff, shadowCoordOut);
#else
    float visibility = 1.0;
#endif
*/
    float visibility = 1.0;

    float ao = texture(aoMap, texCoord).r;

#ifdef DEFERRED


    geoData[0] = vec4(vec4(diffColor, ao));
    geoData[1] = vec4(normalize(viewSpaceNormal),0);
    geoData[2] = vec4(vec3(0,0,0), 1);

#else

    vec3 v = -normalize(viewSpacePosition);
    vec3 l= -viewSpaceLightDirection;
    vec3 n = viewSpaceNormal;


    // TODO: we really need all this?
    float specShiny = 0;
    vec3 specColor = vec3(0);
    float diff=  calcDiff(l,n);

   float spec= calcSpec(l,n,v);

    geoData[0] =vec4(vec3(1.0-ao), 1.0) * aoOnly +
	(1.0 - aoOnly)*calcLighting(
	ambientLight* (1.0 -ao),
	sceneLight,
	specShiny,
	diffColor.xyz,
	specColor.xyz,
	diff,
	spec,
	visibility,
	vec3(0) );
#endif




//    geoData[0] = vec4(vec4(vec3(1,0,0), 1));

}
