layout (location = 0) in  vec2 positionIn;

#include "height_map_lib.glsl"

uniform mat4 mvp;

uniform sampler2D heightMap;

uniform float xzScale;
uniform vec3 offset;
uniform float yScale;

uniform vec2 chunkPos;
uniform float chunks; // num chunks.

void main()
{
    vec3 pos =
	computePos(positionIn, chunkPos, chunks,heightMap,  xzScale, offset, yScale);

    gl_Position = mvp * vec4(pos,1);
}
