layout (location = 0) in  vec3 positionIn;
layout (location = 1)in vec2 texCoordIn;

uniform mat4 mvp;
out vec2 texcoordOut;



void main()
{
    gl_Position = mvp * vec4(positionIn,1);
    texcoordOut = texCoordIn;
}
