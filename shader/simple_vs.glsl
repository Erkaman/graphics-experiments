in  vec3 positionIn;
in  vec3 colorIn;


uniform mat4 proj;
uniform mat4 view;

in vec2 texCoordIn;
out vec2 texCoord;

void main()
{
//    mat4 m = mat4(1.0);
    gl_Position = proj * view * vec4(positionIn,1);
    texCoord = texCoordIn;
}
