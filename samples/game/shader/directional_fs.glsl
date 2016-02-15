#include "shader/lighting_lib.glsl"

in vec2 texCoord;

out vec4 fragmentColor;

uniform sampler2D colorTexture;
uniform sampler2D depthTexture;
uniform sampler2D normalTexture;
uniform sampler2D positionTexture;
uniform sampler2DShadow shadowMap;

uniform vec2 screenSize;

uniform vec3 ambientLight;
uniform vec3 sceneLight;
uniform vec3 viewSpaceLightDirection;
uniform mat4 lightVp;
uniform mat4 inverseViewMatrix;


void main() {

    vec3 viewSpaceNormal = texture(normalTexture, texCoord).xyz;
    vec3 viewSpacePosition = texture(positionTexture, texCoord).xyz;

    vec3 v = -normalize(viewSpacePosition);
    vec3 l= -viewSpaceLightDirection;
    vec3 n = viewSpaceNormal;

    float diff=  calcDiff(l,n);
    float spec= calcSpec(l,n,v);

    vec4 sample = texture(colorTexture, texCoord);
    vec3 diffColor = sample.xyz;
    float ao = sample.w;

    vec3 specColor = vec3(0,0,0);
    float specShiny = 0;

//    float visibility = 1.0;

    float aoOnly =0.0;

    vec4 shadowCoord = (lightVp * (inverseViewMatrix * vec4(viewSpacePosition.xyz,1)));

    float visibility = calcVisibility(shadowMap, diff, shadowCoord);

    visibility = 1.0;

    fragmentColor =vec4(vec3(1.0-ao), 1.0) * aoOnly +
	(1.0 - aoOnly)*calcLighting(
	ambientLight* (1.0 -ao),
	sceneLight,
	specShiny,
	diffColor,
	specColor.xyz,
	diff,
	spec,
	visibility,
	vec3(0) );


    fragmentColor = vec4(vec3(diff) , 1.0);

/*

    vec4 color = texture(randomTexture, screenSize * texCoord / vec2(64) );

    fragmentColor = vec4( color.xyz,1);
*/

/*
      float z = texture(depthTexture, texCoord).r;

      float ndcDepth  =
	  (2.0 * z - 1.1 - 500.0f) /
	  (500.0f - 1.1);
*/

//    vec4 color = texture(colorTexture, texCoord);

//      ndcDepth = ndcDepth / gl_FragCoord.w;

//      fragmentColor = vec4( vec3(ndcDepth*0.5 + 0.5 ) ,1);
}
