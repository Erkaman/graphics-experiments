layout (location = 0)in  vec3 positionIn;
layout (location = 1)in  vec3 normalIn;
layout (location = 2)in vec2 texCoordIn;

uniform mat4 mvp;
uniform mat4 modelViewMatrix;
uniform mat4 normalMatrix;

out vec3 viewSpaceNormal;
out vec3 viewSpacePosition;
//out vec3 vertexColor;
out vec2 texCoord;

out vec3 position;
out vec3 normal;


void main()
{
    gl_Position = mvp * vec4(positionIn,1);

    viewSpaceNormal = normalize((normalMatrix * vec4(normalize(normalIn),0.0)).xyz);

    normal = normalIn;

    viewSpacePosition = (modelViewMatrix * vec4(positionIn, 1.0)).xyz;

//    vertexColor = colorIn.rgb;

    texCoord = texCoordIn;

    position = positionIn;
}
