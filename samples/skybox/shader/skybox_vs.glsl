in  vec3 positionIn;

out vec3 texCoord;

uniform mat4 mvp;

void main()
{
    gl_Position = mvp * vec4(positionIn,1);

    texCoord = positionIn.xyz;
}
