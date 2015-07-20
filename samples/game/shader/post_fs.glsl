out vec4 fragmentColor;
in vec2 texCoord;

uniform sampler2D tex;

/*vec4 vignette(vec4 sample) {
    float attenuation = 5.0f;
    float power = 4.7;

    float delta = distance(texCoord, vec2(0.5f, 0.5f));
    float darkening = 1 - pow(delta, power) * attenuation;


    return sample * vec4(vec3(darkening),1);
    }

vec4 tonemap(vec4 hdr)
{
	float contrast = 6.0f;
	float bias = 1.2f;


	vec3 c = hdr.rgb;
	float low = exp(logLuminance - bias - contrast/2);
	float high = exp(logLuminance - bias + contrast/2);



	vec3 ldr = (hdr.rgb - low) / (high - low);
	return vec4(ldr, hdr.a);

	}*/

void main()
{
    vec2 rcpFrame = 1.0 / textureSize(tex, 0);

    vec4 sample = texture(tex, texCoord);

    sample = 1.0 - exp(sample * -1.4);

    fragmentColor = sample;
}
