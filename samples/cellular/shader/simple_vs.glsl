layout (location = 0)in  vec3 positionIn;
layout (location = 1)in vec2 texCoordIn;
layout (location = 2)in vec3 normalIn;

uniform mat4 mvp;

out vec2 texCoord;
out vec3 normal;

void main()
{
    gl_Position = mvp * vec4(positionIn,1);
    texCoord = texCoordIn;
    normal = normalIn;
}
