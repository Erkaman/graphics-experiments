//layout (location = 0)


layout (location = 0) in  vec3 positionIn;
layout (location = 1) in  float idIn;
layout (location = 2) in vec2 texCoordIn;

#include "height_map_lib.glsl"

uniform mat4 mvp;
uniform mat4 modelViewMatrix;
uniform mat4 normalMatrix;

uniform sampler2D heightMap;

uniform float xzScale;
uniform vec3 offset;
uniform float yScale;
uniform float resolution;

out vec3 viewSpaceNormal;
out vec3 viewSpacePosition;
//out vec3 vertexColor;
out vec2 texCoord;

out vec3 position;


out float id;

out vec3 norm;

vec3 getNormal2(sampler2D hm, vec2 texCoord, float resolution)
{
    float eps = 1.0 / resolution;
    vec3 p = vec3(texCoord.x, 0, texCoord.y);

    //eps on x axis.
//    vec3 va = vec3(2*eps, f(hm,p.x+eps,p.z) - f(hm,p.x-eps,p.z), 0 );

//    vec3 vb = vec3(0, f(hm,p.x,p.z+eps) - f(hm,p.x,p.z-eps), 2*eps );

    // http://stackoverflow.com/questions/5281261/generating-a-normal-map-from-a-height-map
//    vec3 n = normalize(cross(normalize(vb), normalize(va) ));

    vec3 n;

    n.x = f(hm,p.x+eps,p.z) - f(hm,p.x-eps,p.z);
    n.y = 2 * eps;
    n.z = f(hm,p.x,p.z+eps) - f(hm,p.x,p.z-eps);

    return normalize(n);
}

void main()
{
    vec3 pos = computePos(positionIn, heightMap,  xzScale, offset, yScale);

    gl_Position = mvp * vec4(pos,1);

    norm = getNormal(heightMap,positionIn.xz, resolution);

    viewSpaceNormal = normalize((normalMatrix * vec4(normalize(
							 norm
							 ),0.0)).xyz);

    viewSpacePosition = (modelViewMatrix * vec4(pos, 1.0)).xyz;

    texCoord = texCoordIn;

    position = pos;

    id = idIn;
}
