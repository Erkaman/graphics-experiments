layout (location = 0) in vec3 Position;

void main()
{
    gl_Position = vec4(vec3(10,0,10)   /*Position*/, 1.0);
}
