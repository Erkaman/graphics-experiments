in  vec3 positionIn;
in  vec3 normalIn;
in  vec4 colorIn;

in vec2 texCoordIn;

uniform mat4 mvp;
uniform mat4 modelViewMatrix;
uniform mat4 normalMatrix;

out vec3 viewSpaceNormal;
out vec3 viewSpacePosition;
out vec3 vertexColor;
out vec2 texCoord;

out vec3 position;


void main()
{
    gl_Position = mvp * vec4(positionIn,1);

    viewSpaceNormal = normalize((normalMatrix * vec4(normalize(normalIn),0.0)).xyz);

    viewSpacePosition = (modelViewMatrix * vec4(positionIn, 1.0)).xyz;

    vertexColor = colorIn.rgb;

    texCoord = texCoordIn;

    position = positionIn;

//    texCoord = texCoordIn;
}
