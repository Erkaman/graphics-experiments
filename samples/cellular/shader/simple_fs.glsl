#include "shader_lib/lib.glsl"

in vec3 position;

uniform mat4 normalMatrix;

out vec4 fragmentColor;

in vec2 texCoord;

uniform sampler2D tex;

#ifdef NORMAL_MAPPING
uniform sampler2D normalMap;
#endif

in vec3 normal;

#ifdef NORMAL_MAPPING
in vec3 tangent;
#endif


uniform vec3 viewSpaceLightPosition;
in vec3 viewSpaceNormal;
in vec3 viewSpacePosition;


#ifdef NORMAL_MAPPING

vec3 CalcBumpedNormal()
{
    vec3 Normal = normalize(normal);
    vec3 Tangent = normalize(tangent);
    Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
    vec3 Bitangent = cross(Tangent, Normal);
    vec3 BumpMapNormal = texture(normalMap,  vec2(texCoord.x, 1-texCoord.y  )).xyz;
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
    vec3 NewNormal;
    mat3 TBN = mat3(Tangent, Bitangent, Normal);
    NewNormal = TBN * BumpMapNormal;
    NewNormal = normalize(NewNormal);
    return NewNormal;
}

#endif

void main()
{

    vec4 color = texture(tex, vec2(texCoord.x, 1-texCoord.y  )  );


#ifdef NORMAL_MAPPING

    vec4 normal = texture(normalMap, vec2(texCoord.x, 1-texCoord.y  )  );

    vec3 mah_normal =  normalize((normalMatrix * vec4(normalize(CalcBumpedNormal()),0.0)).xyz);

    vec3 shading = phongVertex(color.rgb, mah_normal, viewSpaceLightPosition, viewSpacePosition);

    fragmentColor = vec4( /*shading*/ normal.rgb , color.a);
#else

    vec3 shading = phongVertex(color.rgb, viewSpaceNormal, viewSpaceLightPosition, viewSpacePosition);


    fragmentColor = vec4( shading , color.a);
#endif
}
