in  vec3 positionIn;
in  vec3 normalIn;

uniform mat4 mvp;
uniform mat4 modelViewMatrix;
uniform mat4 normalMatrix;

out vec3 viewSpaceNormal;
out vec3 viewSpacePosition;

void main()
{
    gl_Position = mvp * vec4(positionIn,1);

    viewSpaceNormal = (normalMatrix * vec4(normalIn,0.0)).xyz;
    viewSpacePosition = (modelViewMatrix * vec4(positionIn, 1.0)).xyz;

//    texCoord = texCoordIn;
}
