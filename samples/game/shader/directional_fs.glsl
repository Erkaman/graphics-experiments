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

float toLinearDepth(float depth) {
    float near = 0.1;
    float far = 500.0;

    return 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
}

vec3 getViewSpacePosition(sampler2D depthTexture, vec2 texCoord) {

    float x = texCoord.x * 2 - 1;
    float y = (texCoord.y) * 2 - 1;
//    float z = toLinearDepth(texture(depthTexture, texCoord).r* 2 - 1);
    float z = texture(depthTexture, texCoord).r * 2 - 1;
    vec4 projectedPos = vec4(x, y,z

			     , 1.0f);

    vec4 p =  invProj * projectedPos;

    return p.xyz / p.w;


//    return vec3(z) / 10;
    //   return vec3(texture(depthTexture, texCoord).r);

}

vec3 readNormalTexture(sampler2D normalTexture, vec2 texCoord) {

    vec4 sample = texture(normalTexture, texCoord);
    vec3 viewSpaceNormal = sample.xyz;

    vec3 n = viewSpaceNormal;
    return n;
}

void readSpecularTexture(sampler2D specularTexture, vec2 texCoord, out vec3 specColor, out float specShiny) {

    vec4 sample = texture(specularTexture, texCoord);
    specColor = sample.xyz;
    specShiny = sample.w * 100;
}

void readColorTexture(sampler2D colorTexture, vec2 texCoord, out vec3 diffColor, out float ao) {

    vec4 sample = texture(colorTexture, texCoord);
    diffColor = sample.xyz;
    ao = sample.w;
}


void main() {

    vec3 specColor;
    float specShiny;

    readSpecularTexture(specularTexture, texCoord, specColor, specShiny);


    vec3 viewSpacePosition = getViewSpacePosition(depthTexture, texCoord);

    vec3 v = -normalize(viewSpacePosition);
    vec3 l= -viewSpaceLightDirection;
    vec3 n = readNormalTexture(normalTexture, texCoord);

    float diff=  calcDiff(l,n);
    float spec= calcSpec(l,n,v);

    vec3 diffColor;
    float ao;

    readColorTexture(colorTexture, texCoord, diffColor, ao);

    vec3 specMat = specColor; // + (vec3(1.0) - specColor)  * pow(clamp(1.0 + dot(-v, n), 0.0, 1.0), 5.0);

    float aoOnly =0.0;

    vec4 shadowCoord = (lightVp * (inverseViewMatrix * vec4(viewSpacePosition.xyz,1)));

    float visibility = calcVisibility(shadowMap, diff, shadowCoord);

    fragmentColor =vec4(vec3(1.0-ao), 1.0) * aoOnly +
	(1.0 - aoOnly)*calcLighting(
	ambientLight* (1.0 -ao),
	sceneLight,
	specShiny,
	diffColor,
	specMat,
	diff,
	spec,
	visibility,
	vec3(0) );

}
