layout (location = 0) in  vec3 positionIn;

uniform mat4 mvp;

void main()
{
    gl_Position = mvp * vec4(positionIn,1);
}
