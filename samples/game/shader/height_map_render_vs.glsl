layout (location = 0) in  vec2 positionIn;

#include "shader/height_map_lib.glsl"

uniform mat4 mvp;
uniform mat4 modelViewMatrix;
uniform mat4 normalMatrix;
uniform mat4 lightMvp;

uniform sampler2D heightMap;

uniform float xzScale;
uniform vec3 offset;

uniform vec2 chunkPos;
uniform float chunks; // num chunks.

uniform float yScale;
uniform float resolution;

out vec3 viewSpaceNormal;
out vec3 viewSpacePosition;
out vec2 texCoord;

out vec3 position;

out vec4 shadowCoordOut;

out vec3 outn;

void main()
{
    // pos local in the chunk.
    vec2 localPos = positionIn;

    vec2 globalPos = (positionIn + chunkPos) / chunks;

    vec3 scaledPos = computePos(globalPos, heightMap,  xzScale, offset, yScale);

    gl_Position = mvp * vec4(scaledPos,1);

#ifdef REFRACTION
    gl_ClipDistance[0] = -dot(scaledPos, vec3(0,1,0));
#endif


#ifdef REFLECTION
    gl_ClipDistance[0] = dot(scaledPos, vec3(0,1,0));
#endif

    vec3 norm = getNormal(heightMap,globalPos.xy, resolution);

    viewSpaceNormal = normalize((normalMatrix * vec4(normalize(
							 norm
							 ),0.0)).xyz);

    viewSpacePosition = (modelViewMatrix * vec4(scaledPos, 1.0)).xyz;

    texCoord = globalPos;

    position = scaledPos;

    shadowCoordOut = (lightMvp * vec4(scaledPos,1));

    outn = norm;
}
