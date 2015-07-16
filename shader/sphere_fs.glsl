#include "noise_lib.glsl"

out vec4 fragmentColor;

in vec3 texCoord;

uniform float delta;

// return 1 when x > 0, otherwise return 0.
float when_positive(float x) {
    return max(sign(x), 0);
}

void main()
{
    float y = texCoord.y;


    // make sure y is not negative.
    y = y * when_positive(y);

    vec3 sky = mix(vec3(0.50f, 0.50f, 0.66f), vec3(0.0f, 0.15f, 0.66f), y);

#define CLOUD_SPEED 0.001

    float noise = turbulence(5, vec3(  vec3(

    texCoord.x+delta*CLOUD_SPEED*1.5,
	texCoord.y+delta*CLOUD_SPEED*1.3,
	texCoord.z+delta*CLOUD_SPEED*1.1) *4) * 1, 2, 0.5);

    // decrease the cover of the clouds
    noise -= 0.2;

    noise = noise * when_positive(noise);

    noise = 1 - ( pow(0.1,noise)  * 1 );

    vec3 clouds = vec3(noise);

    float alpha = 0.3;

    fragmentColor = vec4(
	mix(sky, clouds, alpha)
	, 1.0);
}
