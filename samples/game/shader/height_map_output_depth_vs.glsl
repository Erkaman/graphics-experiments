layout (location = 0) in  vec2 positionIn;

#include "shader/height_map_lib.glsl"

uniform mat4 mvp;

uniform sampler2D heightMap;

uniform vec2 chunkPos;

void main()
{
    vec2 localPos = positionIn;

    vec2 globalPos = (positionIn + chunkPos) / numChunks;

    vec3 scaledPos =
	computePos(globalPos,heightMap);

    gl_Position = mvp * vec4(scaledPos,1);
}
