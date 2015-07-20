in  vec3 positionIn;
in vec3 texCoordIn;

uniform mat4 mvp;

out vec3 texCoord;

out vec3 worldSpacePosition;

void main()
{
    gl_Position = mvp * vec4(positionIn,1);

    texCoord = texCoordIn;

    worldSpacePosition = positionIn.xyz;

}