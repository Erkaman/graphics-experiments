
in vec2 texcoordOut;

out vec4 geoData[3];

uniform sampler2D refractionMap;
uniform sampler2D reflectionMap;

in vec4 clipSpace;

void main(void) {

    vec2 ndc = clipSpace.xy / clipSpace.w;
    ndc = ndc * 0.5 + 0.5;

    vec2 refractionTexcoord = ndc;

    vec2 reflectionTexcoord = vec2(ndc.x, 1 - ndc.y);


    vec3 refraction = texture(refractionMap, refractionTexcoord).xyz;

    vec3 reflection = texture(reflectionMap, reflectionTexcoord).xyz;

    vec3 color = mix(refraction, reflection, 0.3);
//    vec3 color = refraction;
//    vec3 color = reflection;

    geoData[0] = vec4(color,1);
}
