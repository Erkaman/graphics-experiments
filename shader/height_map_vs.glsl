in  vec3 positionIn;

uniform mat4 mvp;
uniform mat4 modelView;

//in vec2 texCoordIn;
//out vec2 texCoord;

void main()
{
    gl_Position = mvp * vec4(positionIn,1);
//    texCoord = texCoordIn;
}
