layout (location = 0)in  vec3 positionIn;
layout (location = 1)in vec2 texCoordIn;
layout (location = 2)in vec3 normalIn;

uniform mat4 mvp;
uniform mat4 modelViewMatrix;
uniform mat4 normalMatrix;
uniform vec3 eyePos;
uniform mat4 lightMvp;

out vec2 texcoordOut;


void main()
{
    texcoordOut = texCoordIn;
    gl_Position =mvp * vec4(positionIn,1);
}
