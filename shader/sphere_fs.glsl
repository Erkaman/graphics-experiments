#include "value_noise_lib.glsl"

out vec4 fragmentColor;

in vec3 texCoord;

uniform float delta;

// return 1 when x > 0, otherwise return 0.
float when_positive(float x) {
    return max(sign(x), 0);
}

vec3 make_clouds(vec3 tc) {
    float y = tc.y;


    // make sure y is not negative.
    y = y * when_positive(y);

    vec3 sky = mix(vec3(0.50f, 0.50f, 0.66f), vec3(0.0f, 0.15f, 0.66f), y);

#define CLOUD_SPEED 0.004

    float noise = value_noise_turbulence(7,

					 vec3(tc.x+delta*CLOUD_SPEED*1.5,
	tc.y+delta*CLOUD_SPEED*1.3,
	tc.z+delta*CLOUD_SPEED*1.1) *4

					 , 2, 0.5);

    // decrease the cover of the clouds
    noise -= 0.2;

    noise = noise * when_positive(noise);

    noise = 1 - ( pow(0.1,noise)  * 1 );

    vec3 clouds = vec3(noise);

    float alpha = 0.3;

    return mix(sky, clouds, alpha);
}

void main()
{

    fragmentColor = vec4(
	vec3(1,1,1)
	//	make_clouds(texCoord)
	, 1.0);
}
