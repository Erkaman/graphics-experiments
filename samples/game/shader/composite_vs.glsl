out vec2 texCoord;

const vec2 verts[3] = vec2[]( vec2(-1, -1), vec2(3, -1), vec2(-1, 3) );
const vec2 uvs[3] = vec2[]( vec2(0, 0), vec2(2, 0), vec2(0, 2) );

void main()
{
    vec2 position2 = verts[ gl_VertexID ];
    vec2 texcoord = uvs[ gl_VertexID ];
    texCoord = texcoord;
    gl_Position = vec4( position2, -0.1, 1.0 );

}
