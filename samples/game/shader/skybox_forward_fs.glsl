out vec4 fragmentColor;

in vec3 texCoord;

uniform samplerCube sampler;

void main()
{
    vec3 color = texture(sampler, texCoord).rgb;
    fragmentColor =   vec4(color.rgb, 1.0);
}
