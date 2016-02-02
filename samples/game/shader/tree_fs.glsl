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
in mat4 modelViewMatrixOut;
in vec3 eyePosOut;
in vec4 shadowCoordOut;

uniform mat4 modelViewMatrix;

uniform float normalMap;
uniform float diffMap;
uniform sampler2DShadow shadowMap;

uniform float zNear;
uniform float zFar;

uniform sampler2DArray textureArray;

uniform vec3 ambientLight;
uniform vec3 sceneLight;

out vec4 geoData[3];


uniform vec3 specColor;
uniform float specShiny;

float computeAo() {


    // circle center
    vec3 c = vec3(0,0,1.13129);
    float R = 11.502;
    vec3 n = normalOut;
    vec3 p0 = positionOut;

//    float d = distance(positionOut,c);



    float ro = dot(n,c) - dot(n, p0);
    float h = R - ro;

    // off center distance
    float d = distance(p0,  c + ro * n );

    float vcap = (1.0/3.0) * 3.14 * h*h * (3*R - h);

    float f = (h*h) / (h*h + d*d);

    float a = 0.0006;
    return exp(-1 * a * f * vcap);

//    return vcap / (0.5 * 1.33333 * 3.14 * R*R*R );
}


void main(void) {

    vec4 diffColor=texture(textureArray,vec3(texcoordOut,diffMap) );


    if(diffColor.a < 0.05)
	discard;

    // since it is directional light, minus.
    vec3 lightpos = -viewSpaceLightDirection;
    vec3 p = viewSpacePositionOut; // pixel position in eye space
    vec3 v = -normalize(p); // works, since p is in eye space.
    vec3 n = normalize(vec4(viewSpaceNormalOut,0.0)).xyz; // lol3
    vec3 l= lightpos.xyz; // light vector in eye space.
    n = normalize(n);
    // compute diffuse and specular terms
    float diff=  calcDiff(l,n);
    float spec= calcSpec(l,n,v);
    float visibility = calcVisibility(shadowMap, diff, shadowCoordOut);

    float ao = computeAo();

    geoData[0] = calcLighting(
	ambientLight * ao,
	sceneLight,
	specShiny,
	diffColor.xyz,
	specColor.xyz,
//	diff,
	1.0f,
	spec,
	/*visibility*/ 1.0f);

    geoData[1] = vec4(viewSpaceNormalOut, 0);
    geoData[2] = vec4(viewSpacePositionOut, 0);
}
