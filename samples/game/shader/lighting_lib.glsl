
/*
  l: light vector. vector FROM point TO light.
  n: normal

  and do make sure both are in the same space!
 */
float calcDiff(vec3 l, vec3 n) {
    return clamp(dot(l,n),0,1);
}

/*
  l: light vector. vector FROM point TO light.
  n: normal
  v: view vector. vector FROM point TO camera.

  and do make sure both are in the same space!
 */
float calcSpec(vec3 l, vec3 n, vec3 v) {
    return clamp(dot(normalize(l+v),n),0,1);
}

vec4 calcLighting(

    // TODO: but should not this be the same for all materials.
    // it should be the ambient light of the scene, which is always the same.
    vec3 ambientMat, // ambient material

    // TODO: and maybe also this one should be the same for all objects?
    vec3 diffMat, // diffuse material

    float specShiny,

    vec3 diffColor, // diffuse color from texture
    vec3 specColor, // specular from texture

    float diff, // phong diff
    float spec, // phong spec.

    float visibility // shadowing
    ) {
    vec3 finalcolor=ambientMat*diffColor; // ambient

    finalcolor.xyz+=(diffColor*diffMat*diff * visibility+
			 specColor*pow(spec,specShiny) * visibility );

    return vec4(finalcolor,1.0);
}

float rand(vec4 seed) {
    float dot_product = dot(seed, vec4(12.9898,78.233,45.164,94.673));
    return fract(sin(dot_product) * 43758.5453);
}

vec2 poissonDisk[4] = vec2[](
/*
vec2(-0.640363, 0.639219),
vec2(-0.438874, 0.305672),
vec2(-0.283887, 0.773152),
vec2(-0.943998, 0.093681),
vec2(-0.0211312, 0.0981202),
vec2(0.336444, 0.911633),
vec2(0.244629, 0.519954),
vec2(0.637154, 0.537039),
vec2(0.445192, -0.173428),
vec2(-0.381318, -0.139341),
vec2(0.460371, 0.193586),
vec2(-0.766582, -0.283754),
vec2(0.932469, 0.0815573),
vec2(0.634411, -0.5472),
vec2(0.157984, -0.731492),
vec2(-0.386095, -0.581438)
*/

   vec2( -0.94201624, -0.39906216 ),
   vec2( 0.94558609, -0.76890725 ),
   vec2( -0.094184101, -0.92938870 ),
   vec2( 0.34495938, 0.29387760 )
 );

// calc shadows.
float calcVisibility(sampler2DShadow shadowMap, float diff, vec4 shadowCoord ) {
    float cosTheta = diff;
    float bias = 0.001; //*tan(acos(cosTheta))+0.0005;

    float visibility = 1.0;

#if defined HIGH_QUALITY

    for (int i=0;i<4;i++){

	int index = i;

	vec2 sample = reflect(poissonDisk[index], normalize(gl_FragCoord.xy) );

	 visibility -=
	     0.25 * (1.0-texture( shadowMap, vec3(shadowCoord.xy+sample/3000.0, ( (shadowCoord.z-bias) / shadowCoord.w) )  ));
     }

#else

    visibility -= 1.0* (1.0- texture(shadowMap, vec3(shadowCoord.xy, ( (shadowCoord.z-bias) / shadowCoord.w )  )));
#endif

    return visibility;
}

