// this shader is used for drawing simple primitives such as points, lines, and wireframe cubes.

layout (location = 0)in vec3 positionIn;

uniform mat4 mvp;

void main(void) {
    gl_Position = mvp * vec4(positionIn,1);
}
