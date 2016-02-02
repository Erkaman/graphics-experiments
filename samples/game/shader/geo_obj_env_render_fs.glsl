vec4 sample(sampler2D tex, vec2 uv) {
    return texture(tex, vec2(uv.x, 1.0-uv.y) );
}

#include "shader/lighting_lib.glsl"


in vec3 viewSpacePositionOut;
in vec3 positionOut;
in vec3 normalOut;


in vec3 viewSpaceNormalOut;

in vec2 texcoordOut;

uniform vec3 viewSpaceLightDirection;


uniform float normalMap;
uniform float diffMap;

uniform sampler2DArray textureArray;

out vec4 geoData[1];

uniform vec3 specColor;
uniform float specShiny;

void main(void) {

    vec4 diffColor=texture(textureArray,vec3(texcoordOut,diffMap) );


    // since it is directional light, minus.
    vec3 lightpos = -viewSpaceLightDirection;

    vec4 ambientMat = vec4(vec3(1.0), 1.0);
    vec4 diffMat = vec4(vec3(0.5), 1.0);

    vec3 p = viewSpacePositionOut; // pixel position in eye space

    // view vector: vector from point TO camera. eye space.
    vec3 v = -normalize(p); // works, since p is in eye space.


    vec3 n = normalize(vec4(viewSpaceNormalOut,0.0)).xyz; // lol3

    // if point light:
//    vec3 l=normalize(p-lightpos.xyz); // view vector in eye space.

    vec3 l= lightpos.xyz; // light vector in eye space.

    n = normalize(n);


    // compute diffuse and specular terms
    float diff=  calcDiff(l,n);
    float spec= calcSpec(l,n,v);

    geoData[0] = calcLighting(
	ambientMat.xyz,
	diffMat.xyz,
	specShiny,
	diffColor.xyz,
	specColor.xyz,
	diff,
	spec,
	1.0f);
}
