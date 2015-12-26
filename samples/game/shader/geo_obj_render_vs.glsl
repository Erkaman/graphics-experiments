layout (location = 0)in  vec3 positionIn;
layout (location = 1)in vec2 texCoordIn;
layout (location = 2)in vec3 normalIn;

#if defined NORMAL_MAPPING || defined HEIGHT_MAPPING
layout (location = 3)in vec3 tangentIn;
#endif

uniform mat4 mvp;
uniform mat4 modelViewMatrix;
uniform mat4 normalMatrix;
uniform vec3 eyePos;
uniform mat4 lightMvp;

out vec3 viewSpacePixelPositionOut;

out vec3 normalOut;
#if defined NORMAL_MAPPING || defined HEIGHT_MAPPING
out vec3 tangentOut;
out vec3 bitangentOut;
#endif
out vec2 texcoordOut;

out vec3 viewSpaceLightPositionOut;

out mat4 modelViewMatrixOut;
out vec3 eyePosOut;

out vec4 shadowCoordOut;


void main()
{
    // vertex position
    viewSpacePixelPositionOut = (modelViewMatrix * vec4(positionIn, 1.0)).xyz;

    shadowCoordOut = (lightMvp * vec4(positionIn,1));

    normalOut = normalize(  (normalMatrix * vec4(normalIn, 0.0)).xyz );
#if defined NORMAL_MAPPING || defined HEIGHT_MAPPING
    tangentOut = normalize(  (normalMatrix * vec4(tangentIn, 0.0)).xyz );
    bitangentOut = normalize(     (normalMatrix * vec4(cross(normalIn, tangentIn), 0.0)).xyz    );
#endif
    texcoordOut = texCoordIn;

    gl_Position =mvp * vec4(positionIn,1);

    eyePosOut = eyePos;

    modelViewMatrixOut = modelViewMatrix;
}
