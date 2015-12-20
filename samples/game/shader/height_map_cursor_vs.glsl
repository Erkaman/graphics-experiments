layout (location = 0) in  vec3 positionIn;
layout (location = 1) in  float idIn;
layout (location = 2) in vec2 texCoordIn;

#include "height_map_lib.glsl"

uniform mat4 mvp;

uniform sampler2D heightMap;

uniform float xzScale;
uniform vec3 offset;
uniform float yScale;
uniform float resolution;

vec3 getDir(sampler2D hm, vec2 texCoord, float resolution)
{
    float eps = 1.0 / resolution;
    vec3 p = vec3(texCoord.x, f(hm,  texCoord.x, texCoord.y ), texCoord.y);


    //eps on x axis.
/*
    vec3 va = vec3(2*eps, f(hm,p.x+eps,p.z) - f(hm,p.x-eps,p.z), 0 );

    vec3 vb = vec3(0, f(hm,p.x,p.z+eps) - f(hm,p.x,p.z-eps), 2*eps );
*/

    /*
    vec3 va = vec3(texCoord.x-eps, f(hm,texCoord.x-eps, texCoord.y ) , texCoord.y );

    vec3 vb = vec3(texCoord.x, f(hm,texCoord.x, texCoord.y-eps ) , texCoord.y-eps );

    // http://stackoverflow.com/questions/5281261/generating-a-normal-map-from-a-height-map
    return normalize( va - p );
    */
    return vec3(0);
}

vec3 getCentroid(sampler2D hm, vec2 texCoord, float resolution)
{
    float eps = 1.0 / resolution;
    vec2 p = vec2(texCoord.x, texCoord.y);

    vec3 a1 = vec3(texCoord.x, f(hm,  texCoord.x, texCoord.y ), texCoord.y);
    vec3 a2 = vec3(texCoord.x-eps, f(hm,  texCoord.x-eps, texCoord.y ), texCoord.y);
    vec3 a3 = vec3(texCoord.x, f(hm,  texCoord.x, texCoord.y-eps ), texCoord.y-eps);

    return (a1+a2+a3) / 3.0;

}

void main()
{
    // vec3 dir = getDir(heightMap, positionIn.xz, resolution);

    //   vec3 p = positionIn + dir * (1.0 / (256*2) );

    vec3 p  = getCentroid(heightMap,positionIn.xz, resolution );

    vec3 pos =
	offset + vec3(
	p.x * xzScale,
	p.y*yScale,
	p.z * xzScale);


//	p;//computePos(p, heightMap,  xzScale, offset, yScale);

    gl_Position = mvp * vec4(pos,1);
}
