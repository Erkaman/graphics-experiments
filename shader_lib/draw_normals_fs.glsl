out vec4 fragmentColor;


in Data{
    vec3 color;
} gdata;

void main()
{
    fragmentColor = vec4(gdata.color, 1.0);

}
