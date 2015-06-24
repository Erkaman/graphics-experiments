in  vec3 positionIn;
in  vec3 colorIn;

uniform mat4 m;

void main()
{
//    mat4 m = mat4(1.0);
    gl_Position = m * vec4(positionIn,1);
}
