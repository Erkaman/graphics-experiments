layout (location = 0) in  vec2 positionIn;

#include "shader/height_map_lib.glsl"

uniform mat4 mvp;
uniform mat4 modelViewMatrix;
uniform mat4 normalMatrix;
uniform mat4 lightMvp;
uniform mat4 projectionMatrix;

uniform sampler2D heightMap;

// TODO: OPTIMIZE AWAY uniforms.

uniform vec2 chunkPos;


out vec3 viewSpaceNormal;
out vec3 viewSpacePosition;
out vec2 texCoord;

out vec3 position;

out vec2 scaledTexcoord;


out vec3 outn;

void main()
{
    // TODO: pre-multiply by chunks?
    vec2 globalPos = (positionIn + chunkPos) / numChunks;

    vec3 scaledPos = computePos(globalPos, heightMap);


    const float height = 1.0;
    //  const float height = 1.0;

#ifdef REFRACTION
    gl_ClipDistance[0] = -dot(scaledPos, vec3(0,height-1.0,0));
#endif

#ifdef REFLECTION
       gl_ClipDistance[0] = dot(scaledPos, vec3(0,height-1.0,0));
#endif

    // TODO: can we optimize this?
    vec3 norm = getNormal(heightMap,globalPos.xy);

    /*
    viewSpaceNormal = normalize((normalMatrix * vec4(normalize(
							 norm
							 ),0.0)).xyz);
*/
    viewSpaceNormal = normalize(  (normalMatrix * vec4(norm, 0.0)).xyz );


    viewSpacePosition = (modelViewMatrix * vec4(scaledPos, 1.0)).xyz;

//    gl_Position = mvp * vec4(scaledPos,1);
    gl_Position = projectionMatrix * vec4(viewSpacePosition,1);


    texCoord = globalPos;

    position = scaledPos;

    outn = norm;

    scaledTexcoord = texCoord * resolution * textureScale;

}
