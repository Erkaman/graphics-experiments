in vec3 position;

out vec4 fragmentColor;

in vec2 texCoord;

uniform sampler2D tex;

void main()
{
    fragmentColor = vec4( texture(tex, texCoord).xyz /*vec3(texCoord,0)*/, 1.0);
}
