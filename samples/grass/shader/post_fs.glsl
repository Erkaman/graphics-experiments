out vec4 fragmentColor;
in vec2 texCoord;

uniform sampler2D tex;


void main()
{
    vec2 rcpFrame = 1.0 / textureSize(tex, 0);

    vec4 sample = texture(tex, texCoord);

    fragmentColor = sample;
}
