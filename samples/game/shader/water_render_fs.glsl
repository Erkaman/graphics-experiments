
in vec2 texcoordOut;

out vec4 geoData[3];

uniform sampler2D refractionMap;

in vec4 clipSpace;

void main(void) {

    vec2 ndc = clipSpace.xy / clipSpace.w;
    ndc = ndc * 0.5 + 0.5;

    vec3 color = texture(refractionMap, ndc).xyz;

    geoData[0] = vec4(color,1);
}
