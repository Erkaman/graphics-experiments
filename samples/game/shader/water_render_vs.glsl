layout (location = 0)in  vec3 positionIn;
layout (location = 1)in vec2 texCoordIn;
layout (location = 2)in vec3 normalIn;

uniform mat4 mvp;
uniform mat4 modelViewMatrix;
uniform mat4 normalMatrix;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform vec3 eyePos;

out vec2 texcoordOut;
out vec4 clipSpace;

out vec3 toCameraVector;

out vec3 worldPositionOut;

void main()
{
    texcoordOut = texCoordIn;

    vec4 worldPosition = modelMatrix * vec4(positionIn,1);

    clipSpace = projectionMatrix * viewMatrix * worldPosition;

    gl_Position = clipSpace;

    toCameraVector = normalize(eyePos - worldPosition.xyz);

    worldPositionOut = worldPosition.xyz;
}
