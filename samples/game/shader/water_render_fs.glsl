#include "shader/lighting_lib.glsl"

const float PI = 3.14159265359;

float amplitude[7] = float[7](0.5 / 1.0, 0.5 / 2.0, 0.5 / 2.0, 0.5 / 3.0, 0.5 / 4.0, 0.5 / 5.0, 0.5 / 6.0);
float wavelength[7] = float[7](8.0 * PI / 1.0, 8.0 * PI / 2.0, 8.0 * PI / 3.0, 8.0 * PI / 4.0, 8.0 * PI / 5.0, 8.0 * PI / 6.0, 8.0 * PI / 7.0);
float speed[7] = float[7](1.0, 3.0, 5.0, 7.0, 9.0, 11.0, 13.0);
vec2 direction[7] = vec2[7](vec2(0.59,0.80),vec2(0.98,-0.18),vec2(-0.80,0.58),vec2(-0.91,-0.39),

			    vec2(0.40, 0.91), vec2(-0.45, 0.88), vec2(-0.62, -0.78)   );


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

in vec3 objectSpacePosition;

uniform mat4 modelMatrix;

const float freqScale=0.007;
const float phaseScale=0.01;

float wave(int i, vec2 p) {
    float freq = 2*PI/(wavelength[i] * freqScale);
    float phase = speed[i] * freq * phaseScale;
    float theta = dot(direction[i], p);
    return amplitude[i] * sin(theta * freq + totalDelta * phase);
}

float toLinearDepth(float depth) {
    float near = 0.1;
    float far = 500.0;

    return 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
}

float dWavedx(int i, vec2 p) {
    float freq = 2*PI/(wavelength[i]*freqScale);
    float phase = speed[i] * freq* phaseScale;
    float theta = dot(direction[i], p);
    float A = amplitude[i] * direction[i].x * freq;
    return A * cos(theta * freq + totalDelta * phase);
}

float dWavedy(int i, vec2 p) {
    float freq = 2*PI/(wavelength[i]*freqScale);
    float phase = speed[i] * freq * phaseScale;
    float theta = dot(direction[i], p);
    float A = amplitude[i] * direction[i].y * freq;
    return A * cos(theta * freq + totalDelta * phase);
}


void main(void) {

    vec2 ndc = clipSpace.xy / clipSpace.w;
    ndc = ndc * 0.5 + 0.5;

    vec2 refractionTexcoord = ndc;
    vec2 reflectionTexcoord = vec2(ndc.x, 1 -ndc.y);

    vec2 tc = vec2(
	texcoordOut.x,
	texcoordOut.y);

    tc *= 1.3;

    vec2 distortedTexCoords = texture(textureArray, vec3(tc.x + totalDelta*0.04, tc.y, dudvMap)).rg*0.1;
    distortedTexCoords = tc + vec2(distortedTexCoords.x, distortedTexCoords.y+ totalDelta * 0.03);
    vec2 distort = (texture(textureArray, vec3(distortedTexCoords,dudvMap) ).rg * 2.0 - 1.0) * 0.02;


    refractionTexcoord += distort;
    reflectionTexcoord += distort;




    refractionTexcoord = clamp(refractionTexcoord, 0.001, 1.0 - 0.001);

    vec3 refraction = texture(refractionMap, refractionTexcoord).xyz;
    vec3 reflection = texture(reflectionMap, reflectionTexcoord).xyz;

    vec3 n = texture(textureArray, vec3(distortedTexCoords, normalMap) ).xyz;
    n = vec3(2*n.r - 1.0, n.b, 2*n.g - 1.0);
    n = normalize(n);


    float waterDistance = toLinearDepth(gl_FragCoord.z);
    float floorDistance = toLinearDepth(  texture(depthMap, ndc).r  );
    float waterDepth = floorDistance - waterDistance;
    float a = clamp(waterDepth / 40.0, 0.0, 1.0);

    float ao = 1;
    float id = 2.0f;

    geoData[0] = vec4(packColor( reflection ), packColor(refraction)   );
    geoData[1] = packNormalTexture(n.xyz, a, id);
}
