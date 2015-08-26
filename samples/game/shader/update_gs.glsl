layout(points) in;
layout(points) out;
layout(max_vertices = 30) out;

in float type0[];
in vec3 position0[];
in vec3 velocity0[];
in float age0[];

out float type1;
out vec3 position1;
out vec3 velocity1;
out float age1;

uniform float deltaTime;
uniform float time;
uniform sampler1D randomTexture;
uniform float emitInterval;
uniform float particleLifetime;


uniform vec3 minVelocity;
uniform vec3 maxVelocity;





#define PARTICLE_TYPE_EMITTER 0.0f
#define PARTICLE_TYPE_PARTICLE 1.0f

// given a random value rand, return a number in range [low,high]
float rand_range(float rand, float low, float high) {
    return low + rand * (high - low);
}

vec3 GetRandomDir(float seed, vec3 minVel, vec3 maxVel) {
    vec3 seeds =  texture(randomTexture, seed).xyz;

    return vec3(
	rand_range(seeds.x, minVel.x, maxVel.x),
	rand_range(seeds.y, minVel.y, maxVel.y),
	rand_range(seeds.z, minVel.z, maxVel.z)
	);
}

void main() {
    float age = age0[0] + deltaTime;

    if (type0[0] == PARTICLE_TYPE_EMITTER) {
        if (age >= emitInterval) {
            type1 = PARTICLE_TYPE_PARTICLE;
            position1 = position0[0];

	    velocity1 = GetRandomDir(time, minVelocity, maxVelocity);

            age1 = 0.0;
            EmitVertex();
            EndPrimitive();
            age = 0.0;
	}

        type1 = PARTICLE_TYPE_EMITTER;
        position1 = position0[0];
        velocity1 = velocity0[0];
        age1 = age;
        EmitVertex();
        EndPrimitive();
    }
    else {
        vec3 DeltaP = deltaTime * velocity0[0];
        vec3 DeltaV = deltaTime * vec3(0.0, 0.0, 0.0);

        if (type0[0] == PARTICLE_TYPE_PARTICLE)  {
	        if (age < particleLifetime) {
	            type1 = PARTICLE_TYPE_PARTICLE;
	            position1 = position0[0] + DeltaP;
	            velocity1 = velocity0[0] + DeltaV;
	            age1 = age;
	            EmitVertex();
	            EndPrimitive();
	        } // else particle will die.
        }
    }
}
