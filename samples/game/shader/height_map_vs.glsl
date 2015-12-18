//layout (location = 0)


layout (location = 0) in  vec3 positionIn;
layout (location = 1) in  float idIn;
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

out vec3 norm;

out float id;

// sample heightmap
float f(vec2 texCoord) {
    return texture(heightMap, texCoord).r;
}

float f(float x, float z) {
    return f(vec2(x,z));
}

vec3 getNormal(vec2 texCoord)
{
/*
    float eps = 1.0 / 256.0;
    vec2 p = texCoord;

    vec3 n = vec3( f(p.x-eps,p.y) - f(p.x+eps,p.y),
                         2.0f*eps,
                         f(p.x,p.y-eps) - f(p.x,p.y+eps) );
    return normalize( n );
*/



    float eps = 1.0 / 256.0;
    vec3 p = vec3(texCoord.x, 0, texCoord.y);


    //eps on x axis.
    vec3 va = vec3(2*eps, f(p.x+eps,p.z) - f(p.x-eps,p.z), 0 );

    vec3 vb = vec3(0, f(p.x,p.z+eps) - f(p.x,p.z-eps), 2*eps );

    // http://stackoverflow.com/questions/5281261/generating-a-normal-map-from-a-height-map
    vec3 n = normalize(cross(normalize(vb), normalize(va) ));



//    return normalize(vec3(f(p.x,p.z)  ));

    return n;
//    return n * 0.5 + 0.5;

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

    viewSpaceNormal = normalize((normalMatrix * vec4(normalize(

							 /*normalIn*/ getNormal(positionIn.xz)
							 ),0.0)).xyz);

    viewSpacePosition = (modelViewMatrix * vec4(pos, 1.0)).xyz;

    texCoord = texCoordIn;

    norm = normalize(getNormal(positionIn.xz));
    position = pos;

    id = idIn;
}
