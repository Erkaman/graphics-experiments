

uniform float deltaTime;
uniform float time;
uniform sampler1D randomTexture;
uniform float emitRate;
uniform float baseParticleLifetime;
uniform int emitCount;

uniform float baseStartSize;
uniform float baseEndSize;

uniform vec4 startColor;
uniform vec4 endColor;


uniform vec3 minVelocity;
uniform vec3 maxVelocity;
uniform vec3 emitPosition;
uniform vec3 emitPositionVariance;

uniform float particleLifetimeVariance;
uniform float startSizeVariance;
uniform float endSizeVariance;


out Vertex
{
    float size;
    vec4 color;

} vo;


/*
vec3 GetRandomPosition(float seed) {
    seed += emitPosition.x * emitPosition.y * emitPosition.z;

    vec3 rands =  texture(randomTexture, seed).xyz;
    rands = -1 + 2 * rands; // convert from range [0,1] to range [-1,1]

    return emitPosition + rands * emitPositionVariance;
}


*/

vec3 GetRandomPosition(float seed, vec3 emitPosition, vec3 emitArea) {
    seed += seed * emitPosition.x + emitPosition.y + emitPosition.z;

    vec3 rands =  texture(randomTexture, seed).xyz;
    rands = -1 + 2 * rands; // convert from range [0,1] to range [-1,1]

    return emitPosition + rands * emitArea;
}

float rand_range(float rand, float low, float high) {
    return low + rand * (high - low);
}

float rand_variance(float rand, float base, float variance) {
    return base + rand_range(rand, -1, +1) * variance;
}

float RandVariance(float seed, float base, float variance) {
    float rand =  texture(randomTexture, seed).x;
    return rand_variance(rand, base, variance);
}
/*
vec3 GetRandomColor(float seed) {

    vec3 color = vec3(0.5);
    vec3 colorVariance = vec3(0.5);


    seed += color.x * color.y * color.z;

    vec3 rands =  texture(randomTexture, seed).xyz;
    rands = -1 + 2 * rands; // convert from range [0,1] to range [-1,1]

    return color + rands * colorVariance;
}
*/

vec3 GetRandomDir(float seed, vec3 minVel, vec3 maxVel) {

    vec3 seeds =  texture(randomTexture, seed).xyz;

    return vec3(
	rand_range(seeds.x, minVel.x, maxVel.x),
	rand_range(seeds.y, minVel.y, maxVel.y),
	rand_range(seeds.z, minVel.z, maxVel.z)
	);
}


/*
float rand(float co){
    return fract(sin(dot(vec2(co ,co ) ,vec2(12.9898,78.233))) * 43758.5453);
}

float randRange(float seed, float low, float high) {
	return low + (high - low) * rand(seed);
}


float randVariation(float seed, float val, float variation) {
    return val + randRange(seed, -variation, +variation);
}

vec3 randVector(float seed, vec3 col, vec3 variation) {
    return vec3(
	randVariation(seed, col.x, variation.x),
	randVariation(seed, col.y, variation.y),
	randVariation(seed, col.z, variation.z));
}
*/

float lerp(float minVal, float maxVal, float age, float lifetime) {
      return mix(minVal, maxVal, age / lifetime);
}

vec4 lerp(vec4 minVal, vec4 maxVal, float age, float lifetime) {
      return mix(minVal, maxVal, age / lifetime);
}

vec4 GetColor(float age, float lifetime) {
    return lerp(startColor, endColor, age, lifetime);
}


void main(){
     float id = gl_VertexID;

     float seed = (id) / 200;

    float lifetime =RandVariance(seed, baseParticleLifetime, particleLifetimeVariance);

    // pgen = particle generation
    // every time a particle has outlived its lifespan,
    // it is respawned as a new particle at a new position
    // the generation of this new particle is one plus
    // the generation of the old particle.
    float pgen = float(int(time / lifetime));

    // how long the particle of the current generation has lived.
    float age = time - lifetime * pgen;

    float pseed = id *12.2 + pgen * 50.3;



    float startSize =RandVariance(seed, baseStartSize, startSizeVariance);
    float endSize = RandVariance(seed, baseEndSize, endSizeVariance);

    vec3 velocity = GetRandomDir(pseed, minVelocity, maxVelocity);

    vec3 position = GetRandomPosition(pseed, emitPosition, emitPositionVariance) + velocity * age;


    gl_Position = vec4(vec3(position), 1.0);

    vo.size = lerp(startSize, endSize, age, lifetime);
    vo.color = GetColor(age, lifetime);
}
