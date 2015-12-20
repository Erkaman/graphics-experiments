//layout (location = 0)


layout (location = 0) in  vec3 positionIn;
layout (location = 1) in  float idIn;
layout (location = 2) in vec2 texCoordIn;

//in  vec4 colorIn;

uniform mat4 mvp;

uniform sampler2D heightMap;

flat out float id;

// sample heightmap
float f(vec2 texCoord) {
    return texture(heightMap, texCoord).r;
}

float f(float x, float z) {
    return f(vec2(x,z));
}

void main()
{
    float xzScale = 100.0;
    vec3 offset = vec3(50,0,50);

    vec3 pos = offset + vec3(
	positionIn.x * xzScale,
	f(positionIn.xz)*4,
	positionIn.z * xzScale);

    gl_Position = mvp * vec4(pos,1);


    id = idIn;
}
