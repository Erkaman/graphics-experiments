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

/*out vec3 viewSpaceNormal;
out vec3 viewSpacePosition;
*/
out vec3 lightVec;
out vec3 halfVec;
out vec2 v_texcoord;


#ifdef NORMAL_MAPPING
// we have tangentIn
#else
out vec3 v_normal ;
#endif

void main()
{

	vec3 tmpVec;

	vec3 viewSpacePosition = (modelViewMatrix * vec4(positionIn, 1.0)).xyz;

/*
	#ifdef NORMAL_MAPPING

	vec3 a_bitangent = cross(normalIn,tangentIn); // both tangent and normal are normalized I think


	tmpVec = viewSpaceLightPosition - viewSpacePosition;
//	tmpVec =  lightPosition - a_vertex  ;

		lightVec.x = dot(tmpVec, tangentIn);
		lightVec.y = dot(tmpVec, a_bitangent);
		lightVec.z = dot(tmpVec, normalIn);
		lightVec = normalize(lightVec);

		//tmpVec = cameraPosition - a_vertex ;
		// eye direction.
		tmpVec = viewSpacePosition; // since it is in view space.


		halfVec.x = dot(tmpVec, tangentIn);
		halfVec.y = dot(tmpVec, a_bitangent);
		halfVec.z = dot(tmpVec, normalIn);

		halfVec = normalize(halfVec);

		halfVec = (halfVec + lightVec) /2.0;

		halfVec = normalize(halfVec);

	#else
*/
		//v_normal = normalIn;

	v_normal =  normalize((normalMatrix * vec4(normalIn, 0.0)).xyz);

//	v_normal = normalize((modelViewMatrix * vec4(normalIn, 0.0)).xyz);


		lightVec = viewSpaceLightPosition - viewSpacePosition; //lightPosition - a_vertex ;
		lightVec = normalize(lightVec);

		halfVec  = normalize(viewSpacePosition); //cameraPosition - a_vertex ;
		//	halfVec = (halfVec + lightVec) /2.0;
		//halfVec = normalize(halfVec);

//	#endif
		gl_Position = mvp * vec4(positionIn,1);
		v_texcoord = texCoordIn;


}
