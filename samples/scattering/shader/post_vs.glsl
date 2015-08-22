layout (location = 0)in  vec2 positionIn;
layout (location = 1)in  vec2 texCoordIn;

out vec2 texCoord;

uniform sampler2D tex;

void main()
{
    gl_Position = vec4(positionIn.xy, 0,1);
    texCoord = texCoordIn;
}
