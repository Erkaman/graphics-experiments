layout (location = 0) in  vec2 positionIn;
layout (location = 2) in vec2 texCoordIn;

#include "shader/height_map_lib.glsl"

uniform mat4 mvp;

uniform sampler2D heightMap;

uniform vec3 offset;
uniform float resolution;

uniform vec2 cursorPos;
uniform vec3 cameraPos;

out float isRender;
out float d;

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
    vec3 pos = computePos(
	positionIn + vec2(cursorPos.x / resolution, cursorPos.y / resolution),
	heightMap, offset);

    gl_Position = mvp * vec4(pos,1);

}
