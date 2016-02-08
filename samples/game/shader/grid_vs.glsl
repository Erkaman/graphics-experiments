layout (location = 0)in  vec2 positionIn;


out vec2 pos;
out vec3 color;
uniform sampler2D tex;

void main()
{
    gl_Position = vec4( positionIn, 0.0, 1.0 );

    vec2 texCoord = 0.5 + 0.5 * positionIn;
    color = texture(tex, texCoord).xyz;

    pos = positionIn;
}
