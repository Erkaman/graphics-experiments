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

uniform int bump_on;

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

    vec3 mah_normal =  (normalMatrix * vec4(normalize(CalcBumpedNormal()),0.0)).xyz * 2 -1;

    vec3 n = bump_on==1 ? mah_normal : viewSpaceLightPosition;

    vec3 shading = phongVertex(color.rgb,
			       n, viewSpaceLightPosition, viewSpacePosition);

//    shading = texture(normalMap,  vec2(texCoord.x, 1-texCoord.y  )).xyz;

    fragmentColor = vec4(  shading , color.a);
#else

    vec3 shading = phongVertex(color.rgb, viewSpaceNormal, viewSpaceLightPosition, viewSpacePosition);


    fragmentColor = vec4( shading , color.a);
#endif
}
