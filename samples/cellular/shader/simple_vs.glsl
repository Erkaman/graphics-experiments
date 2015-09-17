layout (location = 0)in  vec3 positionIn;
layout (location = 1)in vec2 texCoordIn;
layout (location = 2)in vec3 normalIn;

#ifdef NORMAL_MAPPING
layout (location = 3)in vec3 tangentIn;
#endif


uniform mat4 mvp;
uniform mat4 modelViewMatrix;
uniform mat4 normalMatrix;

out vec3 viewSpaceNormal;
out vec3 viewSpacePosition;

out vec2 texCoord;
out vec3 normal;

#ifdef NORMAL_MAPPING
out vec3 tangent;
#endif

void main()
{
    gl_Position = mvp * vec4(positionIn,1);
    texCoord = texCoordIn;
    normal = normalIn;

    viewSpaceNormal = normalize((normalMatrix * vec4(normalize(normalIn),0.0)).xyz);
    viewSpacePosition = (modelViewMatrix * vec4(positionIn, 1.0)).xyz;

#ifdef NORMAL_MAPPING
    tangent = tangentIn;
#endif

}
