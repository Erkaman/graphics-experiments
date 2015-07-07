uniform sampler1D permSampler;
uniform sampler1D gradSampler;


vec3 fade(vec3 t) {
  return t * t * t * (t * (t * 6 - 15) + 10); // new curve
}

float perm(float x){
  return texture(permSampler, x / 256.0).a * 256;
}

float grad(float x, vec3 p) {
  return dot(texture(gradSampler, x).xyz, p);
}

float snoise(vec3 p) {

    vec3 P = mod(floor(p), 256.0); // the integer part.
  p -= floor(p); // the fractional part.

  vec3 f = fade(p);


  float A = perm(P.x) + P.y;

  float AA = perm(A) + P.z;

  float AB = perm(A + 1) + P.z;

  float B =  perm(P.x + 1) + P.y;

  float BA = perm(B) + P.z;

  float BB = perm(B + 1) + P.z;



  // AND ADD BLENDED RESULTS FROM 8 CORNERS OF CUBE



  return mix(

    mix(mix(grad(perm(AA), p),

              grad(perm(BA), p + vec3(-1, 0, 0)), f.x),

         mix(grad(perm(AB), p + vec3(0, -1, 0)),

              grad(perm(BB), p + vec3(-1, -1, 0)), f.x), f.y),

    mix(mix(grad(perm(AA + 1), p + vec3(0, 0, -1)),

              grad(perm(BA + 1), p + vec3(-1, 0, -1)), f.x),

         mix(grad(perm(AB + 1), p + vec3(0, -1, -1)),

              grad(perm(BB + 1), p + vec3(-1, -1, -1)), f.x), f.y),

    f.z);

}
