in  vec3 positionIn;
in vec2 texCoordIn;

uniform mat4 mvp;

out vec2 texCoord;

void main()
{
    gl_Position = mvp* vec4(positionIn,1);
    texCoord = texCoordIn;
}
