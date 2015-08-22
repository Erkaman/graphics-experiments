layout (location = 0) in  vec3 positionIn;
layout (location = 1) in vec2 texCoordIn;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;
uniform vec2 offset;


out vec2 texCoord;

void main()
{
    mat4 m = model;

    m[3][0]  =offset.x;
    m[3][1] =0;
    m[3][2]  =offset.y;

    mat4 mv = view * m;

    mv[0][0]  =1;
    mv[0][1] =0;
    mv[0][2]  =0;

    mv[1][0] =0;
    mv[1][1]  =1;
    mv[1][2] =0;

    mv[2][0]  =0;
    mv[2][1] = 0;
    mv[2][2] = 1;

    gl_Position = projection * mv* vec4(positionIn,1);



    texCoord = texCoordIn;
}
