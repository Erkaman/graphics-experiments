layout (location = 0)in  vec3 positionIn;
layout (location = 1)in vec2 texCoordIn;
layout (location = 2)in vec3 normalIn;

#ifdef NORMAL_MAPPING
layout (location = 3)in vec3 tangentIn;
#endif


uniform mat4 mvp;
uniform mat4 modelViewMatrix;
uniform mat4 normalMatrix;

uniform vec3 viewSpaceLightPosition;

out vec3 lightVecOut;
out vec3 eyeVecOut;
out vec2 texcoordOut;


#ifdef NORMAL_MAPPING
// we have tangentIn
#else
out vec3 normalOut;
#endif

void main()
{

    // vertex position
    vec3 viewSpacePosition = (modelViewMatrix * vec4(positionIn, 1.0)).xyz;

#ifdef NORMAL_MAPPING

    vec3 bitangent = cross(normalIn,tangentIn); // both tangent and normal are normalized I think

    vec3 tmpVec = viewSpaceLightPosition - viewSpacePosition;

    lightVecOut.x = dot(tmpVec, tangentIn);
    lightVecOut.y = dot(tmpVec, bitangent);
    lightVecOut.z = dot(tmpVec, normalIn);
    lightVecOut = normalize(lightVecOut);

    //tmpVec = cameraPosition - a_vertex ;
    // eye direction.
    tmpVec = viewSpacePosition; // since it is in view space.

    eyeVecOut.x = dot(tmpVec, tangentIn);
    eyeVecOut.y = dot(tmpVec, bitangent);
    eyeVecOut.z = dot(tmpVec, normalIn);

    eyeVecOut = normalize(eyeVecOut);
#else

    normalOut =  normalize((normalMatrix * vec4(normalIn, 0.0)).xyz);

    lightVecOut = normalize(viewSpaceLightPosition - viewSpacePosition);

    eyeVecOut  = normalize(viewSpacePosition);

#endif
    gl_Position = mvp * vec4(positionIn,1);
    texcoordOut = texCoordIn;
}
