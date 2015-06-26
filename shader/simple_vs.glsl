in  vec3 positionIn;
in  vec3 colorIn;


uniform mat4 mvp;

in vec2 texCoordIn;
out vec2 texCoord;

void main()
{
    gl_Position = mvp * vec4(positionIn,1);
    texCoord = texCoordIn;
}
