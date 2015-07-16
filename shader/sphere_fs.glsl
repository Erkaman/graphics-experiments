#include "noise_lib.glsl"

out vec4 fragmentColor;

in vec3 texCoord;

void main()
{
    float y = texCoord.y;

    if(y < 0) {
	y = 0;
    }

    vec3 sky = mix(vec3(0.50f, 0.50f, 0.66f), vec3(0.0f, 0.15f, 0.66f), y);

//    vec3 grad = mix(vec3(0.81f, 0.38f, 0.66f), vec3(0.0f, 0.15f, 0.66f), y);

    float noise = turbulence(5, vec3(texCoord *4) * 1, 2, 0.5);

    // decrease the cover of the clouds
    noise -= 0.3;

    if(noise < 0) {
	noise = 0;
    }

    noise = 1 - ( pow(0.1,noise)  * 1 );


    vec3 clouds = vec3(noise);



    fragmentColor = vec4(
	clouds + sky
	, 1.0);
}
