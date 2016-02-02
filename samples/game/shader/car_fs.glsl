vec4 sample(sampler2D tex, vec2 uv) {
    return texture(tex, vec2(uv.x, 1.0-uv.y) );
}

#include "shader/lighting_lib.glsl"


uniform samplerCube envMap;

in vec3 viewSpacePositionOut;

in vec3 viewSpaceNormalOut;
#if defined NORMAL_MAPPING || defined HEIGHT_MAPPING
in vec3 tangentOut;
in vec3 bitangentOut;
#endif
in vec2 texcoordOut;

uniform vec3 viewSpaceLightDirection;
in mat4 modelViewMatrixOut;
uniform mat4 inverseViewNormalMatrix;
in vec3 eyePosOut;
in vec4 shadowCoordOut;

uniform float normalMap;
//uniform float diffMap;
uniform vec3 diffColor;


uniform sampler2DShadow shadowMap;

uniform float zNear;
uniform float zFar;

uniform sampler2DArray textureArray;



out vec4 geoData[3];

uniform vec3 specColor;
uniform float specShiny;

void main(void) {

    // since it is directional light, minus.
    vec3 lightpos = -viewSpaceLightDirection;

    vec4 ambientMat = vec4(vec3(0.3), 1.0);
    vec4 diffMat = vec4(vec3(0.5), 1.0);

    vec3 p = viewSpacePositionOut; // pixel position in eye space

    // view vector: vector from point TO camera. eye space.
    vec3 v = -normalize(p); // works, since p is in eye space.

#ifdef HEIGHT_MAPPING

    float depth = 0.1;
    float tile = 1.0;

    // view vector in tangent space
    vec3 s = normalize(vec3(dot(-v,tangentOut.xyz),
		       dot(-v,bitangentOut.xyz),dot(viewSpaceNormalOut,v)));

    // ray direction.
    vec2 ds = (s.xy*depth)/ ( s.z   );
    // ray origin
    vec2 dp = texcoordOut*tile;
    // get intersection distance

    float rayDepth;
    vec2 rayTexcoord;
    rayTrace(textureArray, normalMap,dp,ds,
	     eyePosOut,

	     rayTexcoord, rayDepth);

    vec2 uv = rayTexcoord;
    vec3 n = normalize(texture(textureArray,vec3(uv,normalMap) )).xyz; // lol1

    /*
      Depth correct.
    */
    float planes_x=-zFar/(zFar-zNear);
    float planes_y =-zFar*zNear/(zFar-zNear);
    gl_FragDepth=((planes_x*p.z+planes_y)/-p.z);

#elif defined NORMAL_MAPPING

    vec3 n = texture(textureArray,vec3(texcoordOut,normalMap) ).xyz; // lol2
#else // no normal or height map
    vec3 n = normalize(vec4(viewSpaceNormalOut,0.0)).xyz; // lol3

#endif

    // if point light:
//    vec3 l=normalize(p-lightpos.xyz); // view vector in eye space.

    vec3 l= lightpos.xyz; // light vector in eye space.

#if defined NORMAL_MAPPING || defined HEIGHT_MAPPING

    n=n*2.0-1.0;

     // expand normal to eye space(from tangent space)
    n=normalize(n.x*tangentOut.xyz+
		    n.y*bitangentOut.xyz+n.z*viewSpaceNormalOut);

#else

    n = normalize(n);

#endif

    // compute diffuse and specular terms
    float diff=  calcDiff(l,n);
    float spec= calcSpec(l,n,v);

#if defined HEIGHT_MAPPING
    // shadows. DOES NOT WORK YET.
/*
    float foundRayDepth = rayDepth;

    dp+= ds*foundRayDepth;

    s = normalize(vec3(dot(l,tangentOut.xyz),
		       dot(l,bitangentOut.xyz),dot(normalOut,-l)));

    // move dp, so that we start ray tracing from the light source, instead of from the viewer.
    ds = s.xy*depth/s.z;
    dp-= ds*foundRayDepth;

    ray_intersect_rm(normalMap,dp,ds, rayTexcoord, rayDepth);
    if (rayDepth<foundRayDepth-0.05) { // if pixel in shadow.
	diff*=dot(ambient.xyz,vec3(1.0,1.0,1.0))*0.3333;
	spec=0;
	}*/

#endif

    float visibility = calcVisibility(shadowMap, diff, shadowCoordOut);


    geoData[0] = calcLighting(
	ambientMat.xyz,
	diffMat.xyz,
	specShiny,
	diffColor.xyz,
	specColor.xyz,
	diff,
	spec,
	/*visibility*/ 1.0f);


    vec3 reflectionVector = (inverseViewNormalMatrix *
			     vec4(
				 reflect(-v, viewSpaceNormalOut), 0.0)).xyz;

    vec3 envMapSample = texture(envMap, reflectionVector).rgb;


    geoData[0] = vec4(envMapSample, 1.0);


    geoData[1] = vec4(viewSpaceNormalOut, 0);
    geoData[2] = vec4(viewSpacePositionOut, 0);


    //  fragmentColor = vec4( vec3(cosTheta), 1.0  );
}