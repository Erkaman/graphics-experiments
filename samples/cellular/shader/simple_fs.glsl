in vec3 viewSpacePixelPositionOut;

in vec3 normalOut;
in vec3 tangentOut;
in vec3 bitangentOut;
in vec2 texcoordOut;

in vec3 lightpos;

float saturate(float value) {
    return clamp(value,0.0,1.0);
}

uniform sampler2D normalMap;
uniform sampler2D diffMap;
out vec4 fragmentColor;

float ray_intersect_rm(in sampler2D reliefmap, in vec2 dp, in vec2 ds)
{
    const int linear_search_steps=10;
    const int binary_search_steps=5;
    float depth_step=1.0/linear_search_steps;
    float size=depth_step; // current size of search window
    float depth=0.0; // current depth position
// best match found (starts with last position 1.0)
    float best_depth=1.0;
// search from front to back for first point inside the object
    for ( int i=0; i< linear_search_steps-1;i++){
	depth+=size;
	vec4 t=texture(reliefmap,dp+ds*depth);
	if (best_depth>0.996) // if no depth found yet
	    if (depth >= t.w)
		best_depth=depth; // store best depth
    }
    depth=best_depth;
// search around first point (depth) for closest match
    for ( int i=0; i<binary_search_steps;i++) {
	size*=0.5;
	vec4 t=texture(reliefmap,dp+ds*depth);
	if (depth >= t.w) {
	    best_depth = depth;
	    depth -= 2*size;}
	depth+=size;
    }
    return best_depth;
}


void main(void) {

    float depth = 0.1;
    float tile = 1.0;

    vec4 ambient = vec4(vec3(0.3), 1.0);
    vec4 diffuse = vec4(vec3(1.0), 1.0);
    vec4 specular = vec4(vec3(0.5), 1.0);

    vec4 t,c; vec3 p,v,l,s; vec2 dp,ds,uv; float d;
// ray intersect in view direction
    p = viewSpacePixelPositionOut; // pixel position in eye space
    v = normalize(p); // view vector in eye space
// view vector in tangent space
    // see equation A.43 in RTR.
    s = normalize(vec3(dot(v,tangentOut.xyz),
		       dot(v,bitangentOut.xyz),dot(normalOut,-v)));
// size and start position of search in texture space
//    ds = s.xz*depth/s.confirmed;

    // y.
    // s.z is negative beyond the line! which may explain the weird results.

    s = normalize(s);

    /*
      it may be that s.z is negative for values beyond the line, which would certainly produce weird results.

      // since the distortion goes very deep, it seems that s.z produces huge values for ds.
      // in other words, s.z is very, very small.

      also, are we using a right-handed or left-handed system?

    */

    ds = (s.xy*depth)/ ( s.z   );

    dp = texcoordOut*tile;

// get intersection distance
    d = ray_intersect_rm(normalMap,dp,ds);
// get normal and color at intersection point
    uv=dp+ds*d;
    t=texture(normalMap,uv);
    c=texture(diffMap,uv);
    t.xyz=t.xyz*2.0-1.0; // expand normal to eye space
    // we are multiplying by TBN^-1 here.
    t.xyz=normalize(t.x*tangentOut.xyz+
		    t.y*bitangentOut.xyz+t.z*normalOut);
// compute light direction
    p += v*d*s.z; // p will now become the point where the ray intersect.
    l=normalize(p-lightpos.xyz);

//#ifdef RM DEPTHCORRECT
    const float near = 0.1;
    const float far  = 1024.0;

    float planes_x=-far/(far-near);
    float planes_y =-far*near/(far-near);

    gl_FragDepth=((planes_x*p.z+planes_y)/-p.z);
//#endif



// compute diffuse and specular terms
    float att=saturate(dot(-l,normalOut));
    float diff=saturate(dot(-l,t.xyz));

    // -l, because the light vector goes from the light TO the point!
    float spec=saturate(dot(normalize(-l-v),t.xyz));

    vec4 finalcolor=ambient*c;

    finalcolor.xyz+=att*(c.xyz*diffuse.xyz*diff+
			 specular.xyz*pow(spec,specular.w));
    finalcolor.w=1.0;
    fragmentColor=finalcolor;

}
