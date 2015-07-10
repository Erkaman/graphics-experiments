out vec4 fragmentColor;

uniform vec4 color;

in vec2 texCoord;

uniform sampler2D tex;

void main()
{
    fragmentColor = vec4(1.0, 0.0, 0.0, 1.0);
}
