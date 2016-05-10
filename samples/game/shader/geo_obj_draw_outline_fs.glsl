out vec4 geoData[2];

#include "shader/lighting_lib.glsl"

void main(){
//    fragmentColor = vec3(1,1,0);

    geoData[0] = packColorTexture(vec3(1,1,0), vec3(0.0), 0.0);

    geoData[1] = packNormalTexture(vec3(0.0), 0.0, 0);

}
