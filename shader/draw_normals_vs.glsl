in  vec3 positionIn;
in  vec3 normalIn;

out Data {
    vec3 normal;
    vec4 position;
} vdata;

uniform mat4 mvp;
void main()
{
	gl_Position= vec4(positionIn,1);
	vdata.position = gl_Position;
	vdata.normal = normalIn;

}
