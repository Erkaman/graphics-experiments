


vec4 sample(sampler2D tex, vec2 uv) {
    return texture(tex, vec2(uv.x, 1.0-uv.y) );
}

#ifdef HEIGHT_MAPPING
void rayTrace(
    sampler2D reliefmap,
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
	float height=sample(reliefmap,dp+ds*depth).w;
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

    float nextH	= sample(reliefmap,dp+ds*depth).w - depth; //heightFromTexture - curLayerHeight;
	float prevH	= sample(reliefmap, prevTCoords).w // texture(u_heightTexture, prevTCoords).r
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



in vec3 viewSpacePixelPositionOut;

in vec3 normalOut;
#if defined NORMAL_MAPPING || defined HEIGHT_MAPPING
in vec3 tangentOut;
in vec3 bitangentOut;
#endif
in vec2 texcoordOut;

in vec3 viewSpaceLightPositionOut;
in mat4 modelViewMatrixOut;
in vec3 eyePosOut;

uniform sampler2D normalMap;
uniform sampler2D diffMap;
out vec4 fragmentColor;

uniform vec3 specColor;
uniform float specShiny;

void main(void) {
    // since it is directional light, minus.
    vec3 lightpos = -viewSpaceLightPositionOut;


    vec4 ambient = vec4(vec3(0.3), 1.0);
    vec4 diffuse = vec4(vec3(0.5), 1.0);

    vec3 p = viewSpacePixelPositionOut; // pixel position in eye space
    vec3 v = normalize(p); // view vector in eye space

#ifdef HEIGHT_MAPPING

    float depth = 0.1;
    float tile = 1.0;

    // view vector in tangent space
    vec3 s = normalize(vec3(dot(v,tangentOut.xyz),
		       dot(v,bitangentOut.xyz),dot(normalOut,-v)));

    // ray direction.
    vec2 ds = (s.xy*depth)/ ( s.z   );
    // ray origin
    vec2 dp = texcoordOut*tile;
    // get intersection distance

    float rayDepth;
    vec2 rayTexcoord;
    rayTrace(normalMap,dp,ds,
	     eyePosOut,

	     rayTexcoord, rayDepth);


// get normal and color at intersection point
//    vec2 uv=dp+ds*d;
    vec2 uv = rayTexcoord;
    vec4 finalNormal = normalize(sample(normalMap,uv));
    vec4 finalDiffuse=sample(diffMap, uv );


    // compute light direction
    p += v*rayDepth*s.z;
    vec3 l=normalize(p-lightpos.xyz);

    /*
      Depth correct.
    */
    const float near = 0.1;
    const float far  = 1024.0;
    float planes_x=-far/(far-near);
    float planes_y =-far*near/(far-near);
    gl_FragDepth=((planes_x*p.z+planes_y)/-p.z);

#elif defined NORMAL_MAPPING

    vec4 finalNormal = sample(normalMap,texcoordOut);
    vec4 finalDiffuse=sample(diffMap,texcoordOut);

    vec3 l=normalize(p-lightpos.xyz); // view vector in eye space.
#else // no normal or height map

    vec4 finalNormal = normalize(vec4(normalOut,0.0));
    vec4 finalDiffuse=sample(diffMap,texcoordOut);

    // lightpos in view space
    // p in view space.
    vec3 l=normalize(p-lightpos.xyz); // view vector in eye space.

#endif

#if defined NORMAL_MAPPING || defined HEIGHT_MAPPING

    finalNormal.xyz=finalNormal.xyz*2.0-1.0;

     // expand normal to eye space(from tangent space)
    finalNormal.xyz=normalize(finalNormal.x*tangentOut.xyz+
		    finalNormal.y*bitangentOut.xyz+finalNormal.z*normalOut);

#else

    finalNormal.xyz = normalize(finalNormal.xyz);

#endif


    // compute diffuse and specular terms
    float diff=clamp(dot(-l,finalNormal.xyz),0,1);

    // -l, because the light vector goes from the light TO the point!
    float spec=clamp(dot(normalize(-l-v),finalNormal.xyz),0,1);

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
    vec4 finalcolor=ambient*finalDiffuse;

    finalcolor.xyz+=(finalDiffuse.xyz*diffuse.xyz*diff+
			 specColor*pow(spec,specShiny));
    finalcolor.w=1.0;

    fragmentColor= /*vec4(vec3(specColor*pow(spec,specShiny)),1 );*/   finalcolor;
}
