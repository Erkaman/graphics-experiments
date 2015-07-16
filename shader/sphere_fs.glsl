out vec4 fragmentColor;

in vec3 vertexColor;

void main()
{

    float y = abs(vertexColor.y);

    vec3 grad = mix(vec3(1,1,1), vec3(0,0,1), y);

    fragmentColor = vec4(grad, 1.0);
}
