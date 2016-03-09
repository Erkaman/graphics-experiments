layout (location = 0) in  vec2 positionIn;
layout (location = 1) in vec2 texCoordIn;

#include "height_map_lib.glsl"

uniform mat4 mvp;

uniform sampler2D heightMap;

uniform vec3 offset;

out float id;

void main()
{
    vec3 pos = computePos(positionIn, heightMap, offset);

    gl_Position = mvp * vec4(pos,1);


    id = idIn;
}
