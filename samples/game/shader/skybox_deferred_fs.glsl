out vec4 fragmentColor;

in vec3 texCoord;

uniform samplerCube sampler;

uniform sampler2D depthMap;

uniform float windowWidth;
uniform float windowHeight;

void main()
{
    float depth = texture(depthMap,  vec2(gl_FragCoord.x/windowWidth, gl_FragCoord.y/(windowHeight)) ).r;

    if(depth < 1 )
	discard;

    vec3 color = texture(sampler, texCoord).rgb;
    fragmentColor =   vec4(color.rgb, 1.0);

//    fragmentColor =   vec4(vec3(1,0,0), 1.0);

}
