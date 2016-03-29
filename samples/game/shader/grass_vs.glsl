layout (location = 0) in  vec3 positionIn;
layout (location = 1) in vec2 texCoordIn;
layout (location = 2) in  vec3 normalIn;
layout (location = 3) in vec2 centerPosition;

#include "shader/height_map_lib.glsl"


uniform mat4 mvp;
uniform mat4 modelViewMatrix;
uniform mat4 normalMatrix;
uniform float time;

out vec3 viewSpaceNormal;
out vec3 viewSpacePosition;
//out vec3 vertexColor;

uniform sampler2D heightMap;

out vec2 texCoord;

out vec3 position;

// returns random float in range [-1,1]
float rand(vec2 seed){
    return fract(sin(dot(seed.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float rand(vec3 seed){
    return rand(vec2(seed.x + seed.y * seed.z, seed.x * seed.y + seed.z));
}

// returns random float in range [-1,1]
float rand(float seed) {
    return rand(vec2(seed,seed));
}


// returns random float in range [low,high]
float rand(float seed, float low, float high) {
    float r = (rand(seed) + 1.0) / 2.0; // convert from range [-1,1] to range [0,1].

    return low + r * (high - low);
}

// returns random float in range [low,high]
float rand(vec2 seed, float low, float high) {
    float r = (rand(seed) + 1.0) / 2.0; // convert from range [-1,1] to range [0,1].

    return low + r * (high - low);
}

// returns random float in range [low,high]
float rand(vec3 seed, float low, float high) {
    float r = (rand(seed) + 1.0) / 2.0; // convert from range [-1,1] to range [0,1].

    return low + r * (high - low);
}


void main()
{
    vec3 seed = centerPosition.xyy;

    //if texcoord.y is 0, then move.

//    vec3 pos = positionIn + vec3(centerPosition.x, 15, centerPosition.y);


    vec3 up = getNormal(heightMap, vec2(centerPosition.x / resolution, centerPosition.y / resolution) );

    vec3 right = normalize(cross( up, vec3(1,0,0)   ));
    vec3 forward = normalize( cross(right, up) );

    // in order to do this, we have to make sure that the center of the billboard vertices are the center of the billboard.. Right now, it is not!
    mat3 basis = mat3(up, right, forward);

    vec3 pos = (positionIn) + computePos(
	 vec2(centerPosition.x / resolution, centerPosition.y / resolution),
	heightMap);

//    pos = basis * pos;


    if(texCoordIn.y < 0.1) {

	float xDir =rand(rand(vec2(seed.x, seed.y)), 2*0.05, 2*0.27);
	float zDir =rand(rand(vec2(seed.y, seed.z)), 2*0.05, 2*0.27);

	float period = rand(seed.xyz, 2.0 * 0.60, 2.0 * 0.70);


	pos += vec3(xDir,0, zDir ) * sin(time * period);
    }


    gl_Position = mvp * vec4(pos,1);

    viewSpaceNormal = normalize((normalMatrix * vec4(normalize(normalIn),0.0)).xyz);

    viewSpacePosition = (modelViewMatrix * vec4(positionIn, 1.0)).xyz;

//    vertexColor = colorIn.rgb;

    texCoord = texCoordIn;

    position = positionIn;
}
