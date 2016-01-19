layout (location = 0) in  vec3 positionIn;
layout (location = 1) in  float idIn;
layout (location = 2) in vec2 texCoordIn;

#include "height_map_lib.glsl"

uniform mat4 mvp;
uniform mat4 modelViewMatrix;
uniform mat4 normalMatrix;
uniform mat4 lightMvp;

uniform sampler2D heightMap;

uniform float xzScale;
uniform vec3 offset;
uniform float yScale;
uniform float resolution;

out vec3 viewSpaceNormal;
out vec3 viewSpacePosition;
out vec2 texCoord;

out vec3 position;

out vec4 shadowCoordOut;

void main()
{
    vec3 pos = computePos(positionIn, heightMap,  xzScale, offset, yScale);

    gl_Position = mvp * vec4(pos,1);

    vec3 norm = getNormal(heightMap,positionIn.xz, resolution);

    viewSpaceNormal = normalize((normalMatrix * vec4(normalize(
							 norm
							 ),0.0)).xyz);

    viewSpacePosition = (modelViewMatrix * vec4(pos, 1.0)).xyz;

    texCoord = texCoordIn;

    position = pos;

    shadowCoordOut = (lightMvp * vec4(pos,1));
}
