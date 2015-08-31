layout(points) in;
layout(points) out;
layout(max_vertices = 30) out;

in float type0[];
in vec3 position0[];
in vec3 velocity0[];
in float age0[];
in float size0[];
in vec4 color0[];
in float lifetime0[];
in float startSize0[];
in float endSize0[];

out float type1;
out vec3 position1;
out vec3 velocity1;
out float age1;
out float size1;
out vec4 color1;
out float lifetime1;
out float startSize1;
out float endSize1;

uniform float deltaTime;
uniform float time;
uniform sampler1D randomTexture;
uniform float emitRate;
uniform float particleLifetime;
uniform int emitCount;

uniform float startSize;
uniform float endSize;

uniform vec4 startColor;
uniform vec4 endColor;


uniform vec3 minVelocity;
uniform vec3 maxVelocity;
uniform vec3 emitPosition;
uniform vec3 emitVariance;

uniform float particleLifetimeVariance;
uniform float startSizeVariance;
uniform float endSizeVariance;

#define PARTICLE_TYPE_EMITTER 0.0f
#define PARTICLE_TYPE_PARTICLE 1.0f

float lerp(float minVal, float maxVal, float age, float lifetime) {
      return mix(minVal, maxVal, age / lifetime);
}

vec4 lerp(vec4 minVal, vec4 maxVal, float age, float lifetime) {
      return mix(minVal, maxVal, age / lifetime);
}

// given a random value rand in range [0,1], return a number in range [low,high]
float rand_range(float rand, float low, float high) {
    return low + rand * (high - low);
}

float rand_variance(float rand, float base, float variance) {
    return base + rand_range(rand, -1, +1) * variance;
}

vec3 GetRandomDir(float seed, vec3 minVel, vec3 maxVel) {
    vec3 seeds =  texture(randomTexture, seed).xyz;

    return vec3(
	rand_range(seeds.x, minVel.x, maxVel.x),
	rand_range(seeds.y, minVel.y, maxVel.y),
	rand_range(seeds.z, minVel.z, maxVel.z)
	);
}

vec3 GetRandomPosition(float time, vec3 emitPosition, vec3 emitArea) {
    float seed = time + emitPosition.x + emitPosition.y + emitPosition.z;

    vec3 rands =  texture(randomTexture, seed).xyz;
    rands = -1 + 2 * rands; // convert from range [0,1] to range [-1,1]

    return emitPosition + rands * emitArea;
}

float RandVariance(float seed, float base, float variance) {
    float rand =  texture(randomTexture, seed).x;
    return rand_variance(rand, base, variance);
}


/*float GetSize(float startSize, float endSize, float age, float lifetime) {
    return lerp(startSize, endSize, age, lifetime);
}*/

vec4 GetColor(float age, float lifetime) {
    return lerp(startColor, endColor, age, lifetime);
}


void main() {
    float age = age0[0] + deltaTime;

    if (type0[0] == PARTICLE_TYPE_EMITTER) {
        if (age >= emitRate) {

	    for(int i = 0; i < emitCount; ++i) {

            type1 = PARTICLE_TYPE_PARTICLE;
	    float seed = (time+i)/10.0;
            position1 = GetRandomPosition(seed, emitPosition, emitVariance);

	    velocity1 = GetRandomDir(seed, minVelocity, maxVelocity);

            age1 = 0.0;

	    startSize1 = RandVariance(seed, startSize, startSizeVariance);
	    endSize1 = RandVariance(seed, endSize, endSizeVariance);


	    size1 = lerp(startSize1, endSize1, 0, lifetime0[0]);
	    color1 = GetColor(0, lifetime0[0]);
	    lifetime1 = RandVariance(seed, particleLifetime, particleLifetimeVariance);

            EmitVertex();
            EndPrimitive();
	    }

            age = 0.0;
	}

        type1 = PARTICLE_TYPE_EMITTER;
        position1 = position0[0]; // is basically unused.
        velocity1 = velocity0[0];
        size1 = size0[0];
        color1 = color0[0];
	lifetime1 = lifetime0[0];

        age1 = age;

        EmitVertex();
        EndPrimitive();
    }
    else {
        vec3 DeltaP = deltaTime * velocity0[0];
        vec3 DeltaV = deltaTime * vec3(0.0, 0.0, 0.0);

        if (type0[0] == PARTICLE_TYPE_PARTICLE)  {
	        if (age < lifetime0[0]) {
	            type1 = PARTICLE_TYPE_PARTICLE;
	            position1 = position0[0] + DeltaP;
	            velocity1 = velocity0[0] + DeltaV;
	            age1 = age;
		    size1  = lerp(startSize0[0], endSize0[0], age, lifetime0[0]);
		    color1  = GetColor(age, lifetime0[0]);
		    lifetime1 = lifetime0[0];
		    startSize1 = startSize0[0];
		    endSize1 = endSize0[0];
	            EmitVertex();
	            EndPrimitive();
	        } // else particle will die.
        }
    }
}
