
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
    vec3 ambientMat, // ambient material
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
