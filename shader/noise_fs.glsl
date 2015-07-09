
#include "lib.glsl"

#include "noise_lib.glsl"
#include "noise_3d.glsl"

out vec4 fragmentColor;

uniform vec3 viewSpaceLightPosition;

uniform sampler2D tex;

in vec3 vertexColor;
in vec2 texCoord;

in vec3 viewSpaceNormal;
in vec3 viewSpacePosition;

vec3 sampleDiffuseTexture() {

    vec3 red = vec3(1,0,0);
    vec3 blue = vec3(0,1,0);

    return mix(red, blue, snoise(vec3(10 * texCoord.xy,0)));

//    return texture(tex, texCoord).xyz;
}

void main()
{
    vec3 shading = phongVertex(sampleDiffuseTexture(), viewSpaceNormal, viewSpaceLightPosition, viewSpacePosition);

    // this is our height map color.

    fragmentColor = vec4(shading, 1.0);
}


// https://github.com/okelly4408/MITSubmission/blob/b400fa2f94c41a41a7f694c2326e35287e8b0d33/FromSpaceScatteringWithProceduralClouds/src/Main.java
