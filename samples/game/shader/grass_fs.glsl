#include "shader/lighting_lib.glsl"

out vec4 geoData[2];

in vec3 position;

uniform vec3 viewSpaceLightPosition;
in vec3 viewSpaceNormal;
in vec3 viewSpacePosition;
in float id;

out vec4 fragmentColor;

in vec2 texCoord;

uniform sampler2D tex;

// it's as if tex is not loaded

void main(){


    vec4 sample = texture(tex,texCoord);
    if(sample.a < 0.75)
	discard;


//    vec3 shading = phongVertex(sample.rgb, viewSpaceNormal, viewSpaceLightPosition, viewSpacePosition);


//    sample.xyz = vec3(1);


#ifdef DEFERRED

    geoData[0] = packColorTexture(sample.xyz, vec3(0,0,0), 0);
    geoData[1] = packNormalTexture(vec3(viewSpaceNormal), 0, 0);

#else


#ifdef OUTPUT_ID

    fragmentColor = vec4( float(0),
			  float(0),
			  float(id+1.0), 1 );

#else

    geoData[0] = vec4(sample.xyz,1);
#endif


#endif



//    fragmentColor = vec4(shading, 1.0);
//    fragmentColor = vec4(texCoord,0,1);

}
