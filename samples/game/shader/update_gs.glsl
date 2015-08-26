layout(points) in;
layout(points) out;
layout(max_vertices = 30) out;

in float Type0[];
in vec3 Position0[];
in vec3 Velocity0[];
in float Age0[];

out float Type1;
out vec3 Position1;
out vec3 Velocity1;
out float Age1;

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
    float Age = Age0[0] + deltaTime;

    if (Type0[0] == PARTICLE_TYPE_EMITTER) {
        if (Age >= emitInterval) {
            Type1 = PARTICLE_TYPE_PARTICLE;
            Position1 = Position0[0];

	    Velocity1 = GetRandomDir(time, minVelocity, maxVelocity);

            Age1 = 0.0;
            EmitVertex();
            EndPrimitive();
            Age = 0.0;
	}

        Type1 = PARTICLE_TYPE_EMITTER;
        Position1 = Position0[0];
        Velocity1 = Velocity0[0];
        Age1 = Age;
        EmitVertex();
        EndPrimitive();
    }
    else {
        vec3 DeltaP = deltaTime * Velocity0[0];
        vec3 DeltaV = deltaTime * vec3(0.0, 0.0, 0.0);

        if (Type0[0] == PARTICLE_TYPE_PARTICLE)  {
	        if (Age < particleLifetime) {
	            Type1 = PARTICLE_TYPE_PARTICLE;
	            Position1 = Position0[0] + DeltaP;
	            Velocity1 = Velocity0[0] + DeltaV;
	            Age1 = Age;
	            EmitVertex();
	            EndPrimitive();
	        } // else particle will die.
        }
    }
}
