out vec4 fragmentColor;

uniform vec4 color;

in vec2 texCoord;

uniform sampler2D tex;

void main()
{
    vec4 col = texture(tex, texCoord.xy);

    fragmentColor = col;
}
