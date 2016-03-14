uniform sampler2D gColorMap;

in vec2 TexCoord;
out vec4 fragmentColor;

in vec4 Color;

void main()
{
    fragmentColor =  Color * texture(gColorMap, TexCoord);
}
