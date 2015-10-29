
#include "shader_lib/lib.glsl"


#include "shader_lib/cellular.glsl"

out vec4 fragmentColor;

uniform vec3 viewSpaceLightPosition;

in vec3 vertexColor;
in vec2 texCoord;

in vec3 viewSpaceNormal;
in vec3 viewSpacePosition;

vec3 sampleDiffuseTexture() {

    vec3 red = vec3(1,0,0);
    vec3 blue = vec3(0,1,0);

/*    return mix(red, blue,

      turbulence(3, vec3(texCoord.xy,0) * 3, 2, 0.5)

      );
*/

    vec2 tt = cellular_euclidean(texCoord.xy);

    float f1 = tt.x;
    float f2 = tt.y;


    return vec3( (f2+f1)*0.5 );


//    return texture(tex, texCoord).xyz;
}

void main()
{
    vec3 shading = phongVertex(sampleDiffuseTexture(), viewSpaceNormal, viewSpaceLightPosition, viewSpacePosition);

    // this is our height map color.

    fragmentColor = vec4(shading, 1.0);
}


// https://github.com/okelly4408/MITSubmission/blob/b400fa2f94c41a41a7f694c2326e35287e8b0d33/FromSpaceScatteringWithProceduralClouds/src/Main.java
