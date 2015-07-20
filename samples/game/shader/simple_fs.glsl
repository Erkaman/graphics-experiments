#include "shader_lib/perlin_noise_lib.glsl"

out vec4 fragmentColor;
in vec2 texCoord;

void main()
{

    vec3 red = vec3(1,0,0);
    vec3 blue = vec3(0,1,0);

    vec3 noise = mix(red, blue,
	       turbulence(3, vec3(texCoord.xy,0) * 3, 2, 0.5)
	       );

    noise = vec3(0,0,1);

//    noise -= vec3(10,10,10);

    fragmentColor = vec4(noise, 1.0);
}
