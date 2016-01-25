in vec2 texCoord;

out vec4 fragmentColor;

uniform sampler2D colorTexture;
uniform sampler2D depthTexture;
uniform sampler2D normalTexture;
uniform sampler2D positionTexture;
uniform sampler2D randomTexture;

uniform vec2 screenSize;

uniform float scale;
uniform float bias;
uniform float intensity;
uniform float sampleRad;

vec3 getPosition(in vec2 uv)
{
    return texture(positionTexture,uv).xyz;
}

vec3 getNormal(in vec2 uv)
{
    return texture(normalTexture, uv).xyz;
}

float doAmbientOcclusion(in vec2 uv, in vec2 tcoord, in vec3 p, in vec3 cnorm)
{
    vec3 diff = getPosition(tcoord + uv) - p;
    vec3 v = normalize(diff);
    float d = length(diff)*scale;
    return max(0.0,dot(cnorm,v)-bias)*(1.0/(1.0+d))*intensity;
    /**
       dot(cnorm,v)
       means that those samples straight above the point we're computing AO for,
       will have higher values. Since they will have values (0,1) dot (0,1)

       ( 1.0 / ( 1.0 + d ) )
       attenuates the above effect lineareily with distance

     */

    // if straight over, it will be 1
    // so the ones straight over the point, will block light
    // the most effectively.
}

vec2 getRandom(in vec2 uv)
{
    return vec2(1.0, 0.0);
}

vec2 getRandom2()
{
    return normalize(texture(randomTexture, screenSize * texCoord / vec2(64)).xy );

//    return normalize(texture(randomTexture, texCoord).xy * 2.0f - 1.0f);

}


void main() {


    vec2 uv = texCoord;

    // TODO: this might be slow?
    vec2 vec[8] = vec2[8](vec2(1,0),vec2(-1,0),
			  vec2(0,1),vec2(0,-1),

			  vec2(0.707107, 0.707107),
			  vec2(0.993884, 0.110432),
			  vec2(0.6, 0.8),
			  vec2(0.919145, 0.393919)


	);

    vec3 p = getPosition(uv);
    vec3 n = getNormal(uv);
    vec2 rand = getRandom2();//getRandom(uv);

    float ao = 0.0f;

    /**
       Otherwise we lose the perspective illusion when we move the camera.
     */
    float rad = sampleRad/p.z;


//**SSAO Calculation**//
    int iterations = 1;
    for (int j = 0; j < iterations; ++j)
    {
	vec2 coord1 = reflect(vec[j],rand)*rad;

	// 0.707 = cos(45) = sin(45)
	vec2 coord2 = vec2(coord1.x*0.707 - coord1.y*0.707,
			   coord1.x*0.707 + coord1.y*0.707);

	ao += doAmbientOcclusion(uv,coord1*0.25, p, n);
	ao += doAmbientOcclusion(uv,coord2*0.5, p, n);
	ao += doAmbientOcclusion(uv,coord1*0.75, p, n);
	ao += doAmbientOcclusion(uv,coord2, p, n);
    }
    ao/=float(iterations*4.0);

    fragmentColor = vec4(texture(colorTexture, texCoord).xyz *(1.0-ao),1.0);

/*

    vec4 color = texture(colorTexture, texCoord);
    vec4 color = texture(randomTexture, screenSize * texCoord / vec2(64) );

    fragmentColor = vec4( color.xyz,1);
*/
}
