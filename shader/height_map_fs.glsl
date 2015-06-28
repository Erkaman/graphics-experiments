out vec4 fragmentColor;

//uniform vec4 color;

//in vec2 texCoord;

//uniform sampler2D tex;

void main()
{
    vec3 diffuseColor = vec3(0.33, 0.33, 0.33);

//    vec4 col = texture(tex, texCoord.xy);
    fragmentColor = vec4(diffuseColor, 1.0);//col;
}
