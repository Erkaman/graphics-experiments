in  vec3 positionIn;
in  vec3 normalIn;
//in  vec4 colorIn;

in vec2 texCoordIn;

uniform mat4 mvp;
uniform mat4 modelViewMatrix;
uniform mat4 normalMatrix;
uniform float time;

out vec3 viewSpaceNormal;
out vec3 viewSpacePosition;
//out vec3 vertexColor;
out vec2 texCoord;

out vec3 position;

void main()
{

    //if texcoord.y is 0, then move.

    vec3 pos = positionIn;

    if(texCoordIn.y < 0.1) {
	pos.x += 0.2 * sin(time);
    }

    gl_Position = mvp * vec4(pos,1);

    viewSpaceNormal = normalize((normalMatrix * vec4(normalize(normalIn),0.0)).xyz);

    viewSpacePosition = (modelViewMatrix * vec4(positionIn, 1.0)).xyz;

//    vertexColor = colorIn.rgb;

    texCoord = texCoordIn;

    position = positionIn;
}
