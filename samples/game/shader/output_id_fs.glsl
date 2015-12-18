out vec3 fragmentColor;

in float id;

void main()
{
    fragmentColor = vec3(id+1, 0,0 );
}
