layout (location = 0) in  vec3 positionIn;

//out vec2 texCoord;

uniform mat4 mvp;
uniform mat4 modelViewMatrix;

// light center pos
out vec3 lightCenter;

// light vertex pos
out vec3 lightPos;


void main()
{
/*    vec2 position2 = verts[ gl_VertexID ];
    vec2 texcoord = uvs[ gl_VertexID ];
    texCoord = texcoord;*/

    lightCenter = (modelViewMatrix * vec4(0,0,0,1)).xyz;
    lightPos = (modelViewMatrix * vec4(positionIn,1)).xyz;

    gl_Position = mvp * vec4(positionIn , 1);

}
