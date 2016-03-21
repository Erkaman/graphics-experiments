#include "shader/lighting_lib.glsl"

uniform sampler2D colorTexture;
uniform sampler2D depthTexture;
uniform sampler2D normalTexture;
uniform sampler2D specularTexture;
uniform sampler2DShadow shadowMap;

uniform mat4 toViewSpacePositionMat;
//uniform mat4 invProj;

uniform vec3 color;
uniform mat4 proj;

uniform sampler2D refractionMap;
uniform sampler2D reflectionMap;
uniform mat4 invViewMatrix;
uniform vec3 eyePos;

out vec4 fragmentColor;

in vec3 lightCenter;
in vec3 lightPos;

uniform float radius;

//uniform vec3 sceneLight;

uniform vec2 screenSize;

void main() {

    vec2 texCoord = gl_FragCoord.xy / screenSize;

    vec3 col;

    vec3 viewSpacePosition = getViewSpacePosition(toViewSpacePositionMat, depthTexture, texCoord);

    vec3 v = -(viewSpacePosition);

    vec3 n;
    float id;
    readNormalTexture(normalTexture, texCoord, n, id);

    vec3 lightDist = viewSpacePosition - lightCenter;
    float lightDistLength = length(lightDist);

    // l = normalize(lightDist).
    vec3 l = - lightDist * 1/ ( lightDistLength );

    float diff=  calcDiff(l,n);
    float spec = 0;


    vec3 diffColor;
    float ao;

    readColorTexture(colorTexture, texCoord, diffColor, ao, screenSize.x, screenSize.y);

    float ztest = step(0, radius - lightDistLength );

    vec3 specColor;
    float specShiny;

    readSpecularTexture(specularTexture, texCoord, specColor, specShiny);

    vec3 specMat = specColor;

    float visibility = 1.0;

    // ambient light was handled in separate pass. so should be zero.
    vec3 ambientLight = vec3(0.75);

    vec3 sceneLight = vec3(1);

    vec3 envMapSample = vec3(0);

    if(id == 2.0) {

	waterShader(viewSpacePosition, proj, specColor, refractionMap, reflectionMap, invViewMatrix, eyePos, diffColor, specMat, sceneLight, specShiny, envMapSample, ambientLight);
    }


    vec4 light = calcLighting(

	ambientLight,
	sceneLight,
	    specShiny,
	    diffColor,
	    specMat,
	    diff,
	    spec,
	    visibility,
	    envMapSample );

    light.a = 1.0;

    float atten = clamp(1.0 - lightDistLength / radius, 0,1);

    fragmentColor = (light * vec4(vec3(color),1)) * (ztest * atten);

}
