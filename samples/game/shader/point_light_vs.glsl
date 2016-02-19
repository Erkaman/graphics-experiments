layout (location = 0) in  vec3 positionIn;

//out vec2 texCoord;

uniform mat4 mvp;


void main()
{
/*    vec2 position2 = verts[ gl_VertexID ];
    vec2 texcoord = uvs[ gl_VertexID ];
    texCoord = texcoord;*/
    gl_Position = mvp * vec4(positionIn , 1);

}
