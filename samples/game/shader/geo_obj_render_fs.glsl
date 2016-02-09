vec4 sample(sampler2D tex, vec2 uv) {
    return texture(tex, vec2(uv.x, 1.0-uv.y) );
}

#include "shader/lighting_lib.glsl"

#ifdef HEIGHT_MAPPING
void rayTrace(
    sampler2DArray textureArray,
    float reliefmap,
    vec2 dp, // ray origin in tangent space
    vec2 ds, // ray direction in tangent space.

    vec3 eyePos, // eye position in world space


    out vec2 finalTexCoords, out float parallaxHeight) {

    // TODO: use the angle to determine number of steps!

    float factor = 1.0 - abs(dot( vec3(0, 1, 0) ,  normalize(eyePos) ));

     float linear_search_steps= mix(10.0, 30.0,
					 factor
					 );

    //  const int binary_search_steps=5; // 5

    float size=1.0/linear_search_steps; // current size of search window

    float depth=0.0; // current depth position

    // best match found (starts with last position 1.0)
    float best_depth=1.0;

    // linear search for first point within object.
    for ( int i=0; i< linear_search_steps-1;i++){
	depth+=size;
	float height=texture(textureArray,vec3(dp+ds*depth,reliefmap) ).w;
	if (best_depth>0.996) // if no depth found yet
	    if (depth >= height) {
		best_depth=depth; // store best depth
	    }
    }
    depth=best_depth;


    vec2 currentTextureCoords = dp+ds*depth;

    // previous texture coordinates
    vec2 prevTCoords = dp+ds*(depth-size); // currentTextureCoords + texStep;

    // heights for linear interpolation

    float nextH	= texture(textureArray, vec3(dp+ds*depth, reliefmap)  ).w - depth; //heightFromTexture - curLayerHeight;
    float prevH	= texture(textureArray, vec3(prevTCoords,reliefmap) ).w // texture(u_heightTexture, prevTCoords).r
	    - depth + size;                         // - curLayerHeight + layerHeight;

   // proportions for linear interpolation
   float weight = nextH / (nextH - prevH);

   // interpolation of texture coordinates
   finalTexCoords = prevTCoords * weight + currentTextureCoords * (1.0-weight);

   // interpolation of depth values
   parallaxHeight = depth + prevH * weight + nextH * (1.0 - weight);
       //curLayerHeight + prevH * weight + nextH * (1.0 - weight);





/*
    for ( int i=0; i<binary_search_steps;i++) {
	size*=0.5;
	float height=texture(reliefmap,dp+ds*depth).w;
	if (depth >= height) { // found better depth.
	    best_depth = depth;
	    depth -= 2*size;
	}
	depth+=size;

    }
    return best_depth;
*/

}
#endif

uniform samplerCube envMap;
uniform mat4 inverseViewNormalMatrix;


in vec3 viewSpacePositionOut;

in vec3 viewSpaceNormalOut;
#if defined NORMAL_MAPPING || defined HEIGHT_MAPPING
in vec3 tangentOut;
in vec3 bitangentOut;
#endif
in vec2 texcoordOut;

uniform vec3 viewSpaceLightDirection;
in mat4 modelViewMatrixOut;
in vec3 eyePosOut;
in vec4 shadowCoordOut;

uniform float normalMap;
uniform float diffMap;
uniform vec3 diffColor;
uniform sampler2DShadow shadowMap;

uniform float zNear;
uniform float zFar;

uniform sampler2DArray textureArray;

uniform vec3 ambientLight;
uniform vec3 sceneLight;

out vec4 geoData[3];

uniform vec3 specColor;
uniform float specShiny;
uniform float aoOnly;

#ifdef  AO
in float aoOut;
#endif

void main(void) {

#ifdef DIFF_MAPPING
    vec4 diffColor=texture(textureArray,vec3(texcoordOut,diffMap) );
#else
    vec4 diffColor= vec4(diffColor, 1.0);
#endif

#ifdef ALPHA_MAPPING
    if(diffColor.a < 0.05)
	discard;
#endif


    // since it is directional light, minus.
    vec3 lightpos = -viewSpaceLightDirection;

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



#ifdef SPECULAR_LIGHT
    float spec= calcSpec(l,n,v);
#else
    float spec= 0;
#endif


#ifdef DIFFUSE_LIGHT
    // compute diffuse and specular terms
    float diff=  calcDiff(l,n);
#else
    float diff=  0;
#endif


#ifdef SHADOW_MAPPING
    float visibility = calcVisibility(shadowMap, diff, shadowCoordOut);
#else
    float visibility = 1.0;
#endif



#ifdef ENV_MAPPING
    vec3 reflectionVector = (inverseViewNormalMatrix *
			     vec4(
				 reflect(-v, viewSpaceNormalOut), 0.0)).xyz;


//    float mipmapLevel = textureQueryLod(envMap, reflectionVector).x;

    float eps = 0.01;

#ifdef HIGH_QUALITY
    vec3 envMapSample =
	texture(envMap, reflectionVector).rgb +
	texture(envMap, reflectionVector+vec3(eps,0,0)).rgb +
	texture(envMap, reflectionVector+vec3(0,eps,0)).rgb +
	texture(envMap, reflectionVector+vec3(0,0,eps)).rgb +

	texture(envMap, reflectionVector+vec3(-eps,0,0)).rgb +
	texture(envMap, reflectionVector+vec3(0,-eps,0)).rgb +
	texture(envMap, reflectionVector+vec3(0,0,-eps)).rgb;
     envMapSample *= 1.0 / 7.0;
#else
    vec3 envMapSample = texture(envMap, reflectionVector).rgb;
#endif



#else
    vec3 envMapSample = vec3(0);
#endif

//  vec3 envMapSample = vec3(0);

#ifdef FRESNEL
    vec3 specMat =
	 specColor +
	(vec3(1.0) - specColor)  * pow(clamp(1.0 + dot(-v, n), 0.0, 1.0), 5.0);
#else
    vec3 specMat = specColor;
#endif

#ifdef AO
    vec3 ambient = ambientLight * aoOut;
#else
    vec3 ambient = ambientLight;
#endif

    geoData[0] = aoOnly * vec4(vec3(ambient),1.0) + (1.0 - aoOnly)* calcLighting(
	ambient,
	sceneLight,
	specShiny,
	diffColor.xyz,
	specMat,
	diff,
	spec,
	visibility,
	envMapSample);


//    geoData[0] = vec4(, 1.0);

    /*
#ifdef ENV_MAPPING
    geoData[0] = vec4(envMapSample, 1.0);
#endif
    */


    geoData[1] = vec4(viewSpaceNormalOut, 0);
    geoData[2] = vec4(viewSpacePositionOut, 0);




    //  fragmentColor = vec4( vec3(cosTheta), 1.0  );
}
