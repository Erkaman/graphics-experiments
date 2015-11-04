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

out vec3 viewSpacePixelPositionOut;

out vec3 normalOut;
out vec3 tangentOut;
out vec3 bitangentOut;
out vec2 texcoordOut;

out vec3 lightpos;

#ifdef NORMAL_MAPPING
// we have tangentIn
#else
out vec3 normalOut;
#endif

void main()
{
    //viewSpaceLightPositionOut = viewSpaceLightPosition;

    // vertex position
    viewSpacePixelPositionOut = (modelViewMatrix * vec4(positionIn, 1.0)).xyz;

    lightpos = viewSpaceLightPosition;

    normalOut = normalize(normalIn);
    tangentOut = normalize(tangentIn);
    bitangentOut = normalize(cross(normalIn,tangentIn));

    texcoordOut = texCoordIn;



//#ifdef NORMAL_MAPPING
/*
    vec3 bitangent = cross(normalIn,tangentIn); // both tangent and normal are normalized I think


    vec3 tmpVec = viewSpaceLightPosition - viewSpacePosition;

    lightVecOut.x = dot(tmpVec, tangentIn);
    lightVecOut.y = dot(tmpVec, bitangent);
    lightVecOut.z = dot(tmpVec, normalIn);
    lightVecOut = normalize(lightVecOut);

    viewSpaceEyeVec = normalize(viewSpacePosition);

    // eyeVec = viewSpacePosition, since the position is in view space(so the camera is at (0,0,0) )
    // we convert to eyeVec to tangent space.
    eyeVecOut = vec3(
	dot(viewSpacePosition, tangentIn),
	dot(viewSpacePosition, bitangent),
	dot(viewSpacePosition, normalIn)
	);

    eyeVecOut = normalize(eyeVecOut);
/*#else

    normalOut =  normalize((normalMatrix * vec4(normalIn, 0.0)).xyz);

    lightVecOut = normalize((normalMatrix * vec4(, 0.0)).xyz);
//    lightVecOut = normalize(viewSpaceLightPosition - viewSpacePosition);

    eyeVecOut  = normalize(viewSpacePosition);

    #endif*/
    gl_Position = mvp * vec4(positionIn,1);
}
