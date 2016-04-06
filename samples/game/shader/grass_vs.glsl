layout (location = 0) in  vec3 positionIn;
layout (location = 1) in vec3 texCoordIn;
layout (location = 2) in  vec3 normalIn;
layout (location = 3) in vec3 centerPosition;

#include "shader/height_map_lib.glsl"


uniform mat4 mvp;
uniform mat4 modelViewMatrix;
uniform mat4 normalMatrix;
uniform float time;

out vec3 viewSpaceNormal;
out vec3 viewSpacePosition;
//out vec3 vertexColor;

uniform sampler2D heightMap;
uniform sampler2D meanWindTex;
uniform sampler2D turbWindTex;


out vec2 texCoord;

out vec3 position;
out float id;

uniform vec2 cameraPos;
uniform vec3 cameraDir;

out vec3 color;


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
    vec3 seed = vec3(0.02 * centerPosition.z * centerPosition.xy / resolution, centerPosition.z * 0.03232);


    float h2 = texCoordIn.z;


    id = centerPosition.z;

    //if texcoord.y is 0, then move.

//    vec3 pos = positionIn + vec3(centerPosition.x, 15, centerPosition.y);


    vec3 up = getNormal(heightMap, vec2(centerPosition.x / resolution, centerPosition.y / resolution) );

    vec3 right = normalize(cross( up, vec3(1,0,0)   ));
    vec3 forward = normalize( cross(right, up) );

    // in order to do this, we have to make sure that the center of the billboard vertices are the center of the billboard.. Right now, it is not!
    mat3 basis = mat3(up, right, forward);


    float za = 0.785398;
    float cos_za = /*cos(za)*/ up.y;
    float sin_za = /*sin(za)*/ -up.x;

    mat3 zRot = transpose(mat3(
				cos_za, -sin_za, 0,
				sin_za, +cos_za, 0,
				0     , 0      , 1
			      ));
    float xa = 0.785398;

    float cos_xa = /*cos(xa)*/ up.y;
    float sin_xa = /*sin(xa)*/ up.z;

    mat3 xRot = transpose(mat3(
    			        1     , 0      , 0,
				0     ,cos_xa, -sin_xa,
				0     ,sin_xa, +cos_xa
			      ));


    vec3 rotPos = /*zRot * xRot */ positionIn.xyz;

    vec3 pos = (rotPos + vec3(0,h2,0) ) + computePos(
	 vec2(centerPosition.x / resolution, centerPosition.y / resolution),
	heightMap);

//    pos = basis * pos;

    vec3 normal = normalIn;

    float playerWindStrength = clamp(1.0 - distance(cameraPos.xy, centerPosition.xy)/5, 0, 1);

    /*
    if(texCoordIn.y < 0.1) {

    seed = vec3(
	rand(id / 100),
	rand(32.2*id / 300),
	rand(3232.2*id / 3243));


	float xDir =rand(rand(vec2(seed.x, seed.y)), -2, +2);
	float zDir =rand(rand(vec2(seed.y, seed.z)), -2, +2);

	float period = rand(seed.xyz, 2.0 * 0.60, 2.0 * 0.70);

	vec2 windTexCoord = vec2(centerPosition.x / resolution, centerPosition.y / resolution);

	vec3 meanWind = texture(meanWindTex, windTexCoord).xyz;
	vec3 turbWind = texture(turbWindTex, windTexCoord).xyz;

	vec3 localTurbWind = vec3(xDir,0, zDir ) * sin(time * period);

	vec3 translation =turbWind * 0.5 + meanWind * 0.7 + localTurbWind * 0.1;

//	translation = vec3(0);




	vec2 d = -normalize(cameraPos.xy - centerPosition.xy);


	// affect grass with player pos.
//	translation.xz += d * playerWindStrength * 7;

	pos += translation;
	normal = normalize(normal * (2.5) + translation);

    }
    */


    gl_Position = mvp * vec4(pos,1);

    viewSpaceNormal = normalize((normalMatrix * vec4(normalize(normal),0.0)).xyz);

    viewSpacePosition = (modelViewMatrix * vec4(positionIn, 1.0)).xyz;

//    vertexColor = colorIn.rgb;

    texCoord = texCoordIn.xy;

    position = positionIn;
}
