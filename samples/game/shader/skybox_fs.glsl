out vec4 fragmentColor;

in vec3 texCoord;

uniform samplerCube sampler;

void main()
{
    vec4 color = texture(sampler, texCoord);
    fragmentColor =   vec4(color.rgb, 1.0);

}
