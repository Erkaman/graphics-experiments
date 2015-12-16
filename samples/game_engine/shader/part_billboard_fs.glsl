uniform sampler2D gColorMap;

in vec2 TexCoord;
out vec4 FragColor;

in vec4 Color;


void main()
{
    FragColor = Color * texture(gColorMap, TexCoord);

/*    if(FragColor.a < 0.9) {
	discard;
	}*/

/*    if (FragColor.r >= 0.9 && FragColor.g >= 0.9 && FragColor.b >= 0.9) {
        discard;
	}*/

}
