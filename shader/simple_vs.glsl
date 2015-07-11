in  vec2 positionIn;
in  vec2 texCoordIn;

out vec2 texCoord;

uniform mat4 mvp;

void main()
{
    gl_Position = mvp * vec4(positionIn.xy, 0,1);
    texCoord = texCoordIn;
}
