uniform sampler2D permSampler2d;
uniform sampler2D permGradSampler;

vec3 fade(vec3 t) {
    return t * t * t * (t * (t * 6 - 15) + 10); // new curve
}

vec4 perm2d(vec2 p)
{
    return texture(permSampler2d, p);
}

float gradperm(float x, vec3 p)
{
    return dot(vec3(texture(permGradSampler, vec2(x,0.0)).xyz), p);
}

float snoise(vec3 p) {

//    float val = texture(permGradSampler, vec2(p.x,0) ).g;

    vec3 P = mod(floor(p), 256.0);
    p -= floor(p);
    vec3 f = fade(p);

    P = P / 256.0;
    const float one = 1.0 / 256.0;

    vec4 AA = perm2d(P.xy) + P.z;

    return mix( mix( mix( gradperm(AA.x, p ),
			  gradperm(AA.z, p + vec3(-1.0, 0.0, 0.0) ), f.x),
		     mix( gradperm(AA.y, p + vec3(0.0, -1.0, 0.0) ),
			  gradperm(AA.w, p + vec3(-1.0, -1.0, 0.0) ), f.x), f.y),

		mix( mix( gradperm(AA.x+one, p + vec3(0.0, 0.0, -1.0) ),
			  gradperm(AA.z+one, p + vec3(-1.0, 0.0, -1.0) ), f.x),
		     mix( gradperm(AA.y+one, p + vec3(0.0, -1.0, -1.0) ),
			  gradperm(AA.w+one, p + vec3(-1.0, -1.0, -1.0) ), f.x), f.y), f.z);

}


float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float turbulence(int octaves, vec3 P, float lacunarity, float gain)
{
  float sum = 0;
  float scale = 1;
  float totalgain = 1;
  for(int i=0;i<octaves;i++){
    sum += totalgain*snoise(P*scale);
    scale *= lacunarity;
    totalgain *= gain;
  }
  return abs(sum);
}



//maybe we can simd optimzie this shit! like the cellular stuff.
