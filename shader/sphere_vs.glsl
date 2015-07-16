in  vec3 positionIn;
in  vec4 colorIn;

uniform mat4 mvp;

out vec3 vertexColor;

void main()
{
    gl_Position = mvp * vec4(positionIn,1);

    vertexColor = colorIn.rgb;
}
