layout (location = 0)in  vec3 positionIn;

#include "shader/env_map_lib.glsl"

out vec3 texCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform float znear;
uniform float zfar;

uniform vec3 carPos;
uniform float paraboloidDirection;


void main()
{
    vec3 modelSpacePosition = (modelMatrix * vec4(positionIn, 1.0)).xyz;

    vec3 viewSpacePosition = (viewMatrix * vec4(modelSpacePosition, 1.0)).xyz;

#ifdef PARABOLOID
    gl_Position = envMapProject(paraboloidDirection, viewSpacePosition, modelSpacePosition, carPos, zfar, znear);
#else
    gl_Position = projectionMatrix * vec4(viewSpacePosition,1);
#endif

    texCoord = positionIn.xyz;
}
