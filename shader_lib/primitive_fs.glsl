// this shader is used for drawing simple primitives such as points, lines, and wireframe cubes.

out vec4 fragmentColor;

uniform vec3 color;

void main()
{
    fragmentColor = vec4(color, 1.0);
}
