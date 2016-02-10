
in vec2 texcoordOut;

out vec4 geoData[3];

uniform sampler2D refractionMap;
uniform sampler2D reflectionMap;

in vec4 clipSpace;

uniform sampler2DArray textureArray;

uniform float dudvMap;
uniform float normalMap;

uniform float totalDelta;

in vec3 toCameraVector;


void main(void) {

    vec2 ndc = clipSpace.xy / clipSpace.w;
    ndc = ndc * 0.5 + 0.5;

    vec2 refractionTexcoord = ndc;
    vec2 reflectionTexcoord = vec2(ndc.x, 1 -ndc.y);


    vec2 tc = vec2(
	texcoordOut.x + totalDelta * 0.04,
	texcoordOut.y + totalDelta * 0.01);

    vec2 distort = texture(textureArray, vec3(tc, dudvMap) ).rg * 2.0 - 1.0;


    refractionTexcoord += distort * 0.02;
    reflectionTexcoord += distort * 0.02;

    vec3 refraction = texture(refractionMap, refractionTexcoord).xyz;

    vec3 reflection = texture(reflectionMap, reflectionTexcoord).xyz;

    vec3 color;

    float fresnel = dot(
	toCameraVector, vec3(0,1,0));

    color = mix(refraction, 0.4 * reflection, 1.0 - fresnel);

//    color = mix(refraction, 0.4 * reflection, 0.5);

//    color = refraction;
//    color = reflection;

//    color = vec3(texture(textureArray, vec3(texcoordOut, dudvMap) ).rg, 0);

//    color = vec3(toCameraVector);


    geoData[0] = vec4(color,1);
}
