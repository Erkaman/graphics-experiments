#include "perlin_noise_lib.glsl"

out vec4 fragmentColor;
in vec2 texCoord;

uniform sampler2D tex;

void main()
{
    vec2 rcpFrame = 1.0 / textureSize(tex, 0);

    fragmentColor = vec4(

	texture(tex, texCoord).xyz
	/*texCoord*/
	,1);
}
