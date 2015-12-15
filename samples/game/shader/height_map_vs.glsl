//layout (location = 0)


layout (location = 0) in  vec3 positionIn;
layout (location = 1) in  vec3 normalIn;
layout (location = 2) in vec2 texCoordIn;

//in  vec4 colorIn;


uniform mat4 mvp;
uniform mat4 modelViewMatrix;
uniform mat4 normalMatrix;

uniform sampler2D heightMap;


out vec3 viewSpaceNormal;
out vec3 viewSpacePosition;
//out vec3 vertexColor;
out vec2 texCoord;

out vec3 position;

// sample heightmap
float f(vec2 texCoord) {
    return texture(heightMap, texCoord).r;
}

float f(float x, float z) {
    return f(vec2(x,z));
}

vec3 getNormal(vec2 texCoord)
{
    float eps = 0.00001;
    vec2 p = texCoord;

    vec3 n = vec3( f(p.x-eps,p.y) - f(p.x+eps,p.y),
                         2.0f*eps,
                         f(p.x,p.y-eps) - f(p.x,p.y+eps) );
    return normalize( n );
}

void main()
{
    float xzScale = 200.0;
    vec3 offset = vec3(50,0,50);

    vec3 pos = offset + vec3(
	positionIn.x * xzScale,
	f(positionIn.xz) * 5,
	positionIn.z * xzScale);

    gl_Position = mvp * vec4(pos,1);

    viewSpaceNormal = normalize((normalMatrix * vec4(normalize(

							 /*normalIn*/ getNormal(positionIn.xz)
							 ),0.0)).xyz);

    viewSpacePosition = (modelViewMatrix * vec4(pos, 1.0)).xyz;

    texCoord = texCoordIn;

    position = pos;
}
