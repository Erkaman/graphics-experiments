#include "shader/lighting_lib.glsl"

in vec2 texCoord;

out vec4 fragmentColor;

uniform sampler2D colorTexture;
uniform sampler2D depthTexture;
uniform sampler2D normalTexture;
uniform sampler2D specularTexture;
uniform sampler2DShadow shadowMap;

uniform vec2 screenSize;

uniform vec3 ambientLight;
uniform vec3 sceneLight;
uniform vec3 viewSpaceLightDirection;
uniform mat4 lightVp;
uniform mat4 inverseViewMatrix;
uniform mat4 invProj;
uniform mat4 proj;
uniform mat4 inverseViewNormalMatrix;

uniform samplerCube envMap;


void main() {

    vec3 specColor;
    float specShiny;

    readSpecularTexture(specularTexture, texCoord, specColor, specShiny);

    vec3 viewSpacePosition = getViewSpacePosition(invProj, depthTexture, texCoord);

    vec3 v = -normalize(viewSpacePosition);
    vec3 l= -viewSpaceLightDirection;
    vec3 n;
    float id;
    readNormalTexture(normalTexture, texCoord, n, id);

    float diff=  calcDiff(l,n);
    float spec= calcSpec(l,n,v);

    vec3 diffColor;
    float ao;

    readColorTexture(colorTexture, texCoord, diffColor, ao);

    vec3 specMat = specColor; // + (vec3(1.0) - specColor)  * pow(clamp(1.0 + dot(-v, n), 0.0, 1.0), 5.0);
/*    vec3 specMat =
	 specColor +

*/
    float aoOnly =0.0;

    vec4 shadowCoord = (lightVp * (inverseViewMatrix * vec4(viewSpacePosition.xyz,1)));

    float visibility = calcVisibility(shadowMap, diff, shadowCoord);
/*
    if(id == 1.0) {
	fragmentColor = vec4(vec3(1,0,0), 1.0);
    }
*/

    vec3 envMapSample = vec3(0);


    if(id == 1.0) {

	// add fresnel.
	specMat += (vec3(1.0) - specColor)  * pow(clamp(1.0 + dot(-v, n), 0.0, 1.0), 5.0);

	vec3 reflectionVector = (inverseViewNormalMatrix *
				 vec4(
				     reflect(-v, n.xyz ), 0.0)).xyz;

	envMapSample = texture(envMap, reflectionVector).rgb;
    }

    fragmentColor =vec4(vec3(1.0-ao), 1.0) * aoOnly +
	(1.0 - aoOnly)*calcLighting(
	    ambientLight,
	    sceneLight,
	    specShiny,
	    diffColor,
	    specMat,
	    diff,
	    spec,
	    visibility,
	    envMapSample );


//    fragmentColor = vec4( vec3(specMat), 1.0 );


}
