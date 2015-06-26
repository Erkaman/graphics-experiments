out vec4 fragmentColor;

uniform vec4 color;

in vec2 texCoord;

uniform sampler2D tex;

void main()
{
    vec4 col = texture(tex, texCoord.xy);

    fragmentColor = col; //texture(tex, vec2(0.0,0.0)); //vec4(texCoord.x, texCoord.y,1,0);
}
