layout (location = 0) in  vec3 positionIn;

#include "height_map_lib.glsl"

uniform mat4 mvp;

uniform sampler2D heightMap;

uniform float xzScale;
uniform vec3 offset;
uniform float yScale;

void main()
{
    vec3 pos = computePos(positionIn, heightMap,  xzScale, offset, yScale);

    gl_Position = mvp * vec4(pos,1);
}