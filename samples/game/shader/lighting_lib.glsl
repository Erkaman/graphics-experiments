
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

// calc shadows.
float calcVisibility(sampler2DShadow shadowMap, float diff, vec4 shadowCoord ) {
    float cosTheta = diff;
    float bias = 0.001*tan(acos(cosTheta))+0.001;

    float visibility = 1.0;

    /*
     for (int i=0;i<4;i++){

	 visibility -=
	     0.2 * (1.0-texture( shadowMap, vec3(shadowCoordOut.xy+poissonDisk[i]/700.0, ( (shadowCoordOut.z-bias) / shadowCoordOut.w) )  ));
     }*/

    visibility -= 0.5* (1.0- texture(shadowMap, vec3(shadowCoord.xy, ( (shadowCoord.z-bias) / shadowCoord.w )  )));

    return visibility;
}
