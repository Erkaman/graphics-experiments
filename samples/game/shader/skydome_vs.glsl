layout (location = 0) in  vec3 positionIn;
layout (location = 1) in vec3 texCoordIn;

uniform mat4 mvp;

out vec3 texCoord;

void main()
{
    gl_Position = mvp * vec4(positionIn,1);

    texCoord = texCoordIn;
}
