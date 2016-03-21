
#include "shader/lighting_lib.glsl"


in vec2 texcoordOut;

out vec4 geoData[3];

uniform sampler2D refractionMap;
uniform sampler2D reflectionMap;
uniform sampler2D depthMap;

in vec4 clipSpace;

uniform sampler2DArray textureArray;
uniform vec4 lightDirection;

uniform float dudvMap;
uniform float normalMap;

uniform float totalDelta;

in vec3 toCameraVector;

uniform vec3 sceneLight;

float toLinearDepth(float depth) {
    float near = 0.1;
    float far = 500.0;

    return 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
}

void main(void) {

    vec2 ndc = clipSpace.xy / clipSpace.w;
    ndc = ndc * 0.5 + 0.5;

    vec2 refractionTexcoord = ndc;
    vec2 reflectionTexcoord = vec2(ndc.x, 1 -ndc.y);

    vec2 tc = vec2(
	texcoordOut.x,
	texcoordOut.y);

    tc *= 0.9;

    vec2 distortedTexCoords = texture(textureArray, vec3(tc.x + totalDelta*0.04, tc.y, dudvMap)).rg*0.1;
    distortedTexCoords = tc + vec2(distortedTexCoords.x, distortedTexCoords.y+ totalDelta * 0.03);
    vec2 distort = (texture(textureArray, vec3(distortedTexCoords,dudvMap) ).rg * 2.0 - 1.0) * 0.02;


    refractionTexcoord += distort;
    reflectionTexcoord += distort;

    refractionTexcoord = clamp(refractionTexcoord, 0.001, 1.0 - 0.001);

    vec3 refraction = texture(refractionMap, refractionTexcoord).xyz;
    vec3 reflection = texture(reflectionMap, reflectionTexcoord).xyz;


    //vec2 distort = texture(textureArray, vec3(tc, dudvMap) ).rg * 2.0 - 1.0;


    // SHOULD BE ENOUGH IF just send in distortet text coords.

//    refraction = clamp(refraction, 0.001, 1.0 - 0.001);

    vec3 n = texture(textureArray, vec3(distortedTexCoords, normalMap) ).xyz;
    n = vec3(2*n.r - 1.0, n.b, 2*n.g - 1.0);
    n = normalize(n);

    float waterDistance = toLinearDepth(gl_FragCoord.z);
    float floorDistance = toLinearDepth(  texture(depthMap, ndc).r  );
    float waterDepth = floorDistance - waterDistance;
    float a = clamp(waterDepth / 3.0, 0.0, 1.0);

    float ao = 1;
    float id = 2.0f;

    geoData[0] = vec4(refraction,ao);

    geoData[1] = vec4(n, id);
    geoData[2] = vec4(reflection, 1);

/*
  for water shader.
    vec4(refrac.y, refrac.cbcy, reflec.y, reflec.cbcy)
    vec4( nx, ny, id, ao  )
*/

/*
*/
}
