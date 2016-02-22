#include "shader/lighting_lib.glsl"

uniform sampler2D colorTexture;
uniform sampler2D depthTexture;
uniform sampler2D normalTexture;
uniform sampler2D specularTexture;
uniform sampler2DShadow shadowMap;
uniform mat4 invProj;

out vec4 fragmentColor;

in vec3 lightCenter;
in vec3 lightPos;

uniform float radius;

uniform vec3 ambientLight;
uniform vec3 sceneLight;

uniform vec2 screenSize;

void main() {

    vec2 texCoord = gl_FragCoord.xy / screenSize;

    vec3 col = vec3(1,0,0);


    col = vec3(1,0,0);

    vec3 viewSpacePosition = getViewSpacePosition(invProj, depthTexture, texCoord);

    vec3 v = -normalize(viewSpacePosition);
    vec3 n = readNormalTexture(normalTexture, texCoord);
    vec3 lightDist = viewSpacePosition - lightCenter;
    vec3 l = normalize(lightDist);

    float diff=  calcDiff(l,n);
    float spec = 0;


    vec3 diffColor;
    float ao;

    readColorTexture(colorTexture, texCoord, diffColor, ao);


    float ztest = step(0, radius - length(lightDist));

    vec3 specColor;
    float specShiny;

    readSpecularTexture(specularTexture, texCoord, specColor, specShiny);


    vec3 specMat = specColor; // + (vec3(1.0) - specColor)  * pow(clamp(1.0 + dot(-v, n), 0.0, 1.0), 5.0);


//    col *= length(viewSpacePosition - lightCenter) / 15;

//    col = viewSpacePosition;

//    col *= (length(lightPos - lightCenter) ) / 5;

    col =
	//vec3(1,0,0)
	n

	* ztest;

    float visibility = 1.0;

    vec4 light = calcLighting(
	    ambientLight,
	    sceneLight,
	    specShiny,
	    diffColor,
	    specMat,
	    diff,
	    spec,
	    visibility,
	    vec3(0) );

    light.a = 1.0;

    float atten = clamp(1.0 - length(lightDist) / radius, 0,1);
    atten = 1.0;

    fragmentColor = light * vec4(1,0,0,1) * ztest * atten;
}
