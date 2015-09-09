in vec3 position;

out vec4 fragmentColor;

in vec2 texCoord;

uniform sampler2D tex;

in vec3 normal;

void main()
{
    fragmentColor = vec4(  texture(tex, vec2(texCoord.x, 1-texCoord.y  )  ).rgb, 1.0);
}
