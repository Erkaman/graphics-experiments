layout (location = 0)in  vec3 positionIn;
layout (location = 1)in vec2 texCoordIn;
layout (location = 2)in vec3 normalIn;

uniform mat4 mvp;
uniform mat4 modelViewMatrix;
uniform mat4 normalMatrix;

out vec2 texcoordOut;
out vec4 clipSpace;

void main()
{
    texcoordOut = texCoordIn;

    clipSpace = mvp * vec4(positionIn,1);

    gl_Position = clipSpace;
}
