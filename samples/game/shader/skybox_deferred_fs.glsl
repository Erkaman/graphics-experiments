out vec4 fragmentColor;

in vec3 texCoord;

uniform samplerCube sampler;

uniform sampler2D depthMap;

uniform float windowWidth;
uniform float windowHeight;

float toLinearDepth(float depth) {
    float near = 0.1;
    float far = 500.0;

    return 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
}


void main()
{
    float depth = texture(depthMap,  vec2(gl_FragCoord.x/windowWidth, gl_FragCoord.y/(windowHeight)) ).r;

    if(depth < 1.0 )
	discard;

    depth = toLinearDepth(depth);

    vec3 color = texture(sampler, texCoord).rgb;
    fragmentColor =   vec4(color.rgb, 1.0);
}
