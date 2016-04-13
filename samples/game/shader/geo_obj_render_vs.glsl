#include "shader/env_map_lib.glsl"


#ifdef AO
layout (location = 0)in  vec4 positionIn;
#else
layout (location = 0)in  vec3 positionIn;
#endif

layout (location = 1)in vec2 texCoordIn;
layout (location = 2)in vec3 normalIn;


#if defined NORMAL_MAPPING || defined HEIGHT_MAPPING
layout (location = 3)in vec3 tangentIn;
#endif

uniform mat4 mvp;
uniform mat4 modelViewMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 normalMatrix;
uniform vec3 eyePos;
uniform mat4 lightMvp;

out vec3 viewSpacePositionOut;

out vec3 viewSpaceNormalOut;
#if defined NORMAL_MAPPING || defined HEIGHT_MAPPING
out vec3 tangentOut;
out vec3 bitangentOut;
#endif
out vec2 texcoordOut;

out vec3 viewSpaceLightPositionOut;

out mat4 modelViewMatrixOut;
out vec3 eyePosOut;

out vec4 shadowCoordOut;

uniform float znear;
uniform float zfar;

uniform vec3 carPos;
uniform float paraboloidDirection;


#ifdef AO
out float aoOut;
#endif

void main()
{
    // vertex position
    vec3 modelSpacePosition = (modelMatrix * vec4(positionIn.xyz, 1.0)).xyz;

    viewSpacePositionOut = (viewMatrix * vec4(modelSpacePosition, 1.0)).xyz;

    shadowCoordOut = (lightMvp * vec4(positionIn.xyz,1));

    viewSpaceNormalOut = normalize(  (normalMatrix * vec4(normalIn, 0.0)).xyz );
#if defined NORMAL_MAPPING || defined HEIGHT_MAPPING
    tangentOut = normalize(  (normalMatrix * vec4(tangentIn, 0.0)).xyz );
    bitangentOut = normalize(     (normalMatrix * vec4(cross(normalIn, tangentIn), 0.0)).xyz    );
#endif
    texcoordOut = texCoordIn;


#ifdef PARABOLOID
    //gl_Position =mvp * vec4(positionIn.xyz,1);



    gl_Position = envMapProject(paraboloidDirection, viewSpacePositionOut, modelSpacePosition, carPos, zfar, znear);
#else
    gl_Position =mvp * vec4(positionIn.xyz,1);
#endif

    eyePosOut = eyePos;

    modelViewMatrixOut = modelViewMatrix;
#ifdef AO
    aoOut = positionIn.w;
#endif
}
