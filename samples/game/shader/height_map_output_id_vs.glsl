layout (location = 0) in  vec3 positionIn;
layout (location = 1) in  float idIn;
layout (location = 2) in vec2 texCoordIn;

#include "height_map_lib.glsl"

uniform mat4 mvp;

uniform sampler2D heightMap;

uniform float xzScale;
uniform vec3 offset;
uniform float yScale;

flat out float id;

void main()
{
    vec3 pos = computePos(positionIn, heightMap,  xzScale, offset, yScale);

    gl_Position = mvp * vec4(pos,1);


    id = idIn;
}
