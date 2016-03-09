layout (location = 0) in  vec2 positionIn;
layout (location = 2) in vec2 texCoordIn;

#include "shader/height_map_lib.glsl"

uniform mat4 mvp;

uniform sampler2D heightMap;

uniform vec2 cursorPos;
uniform vec3 cameraPos;

out float isRender;
out float d;


void main()
{
    vec3 pos = computePos(
	positionIn + vec2(cursorPos.x / resolution, cursorPos.y / resolution),
	heightMap);

    gl_Position = mvp * vec4(pos,1);

}
