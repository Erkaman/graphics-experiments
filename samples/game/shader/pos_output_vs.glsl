//layout (location = 0)in vec4 positionIn;
layout (location = 0)in vec3 positionIn;
layout (location = 1)in vec2 texCoordIn;
layout (location = 2)in vec3 normalIn;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 outPos;

void main() {
    /*
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(positionIn.xyz, 1.0);
    outPos = vec3(viewMatrix * modelMatrix * vec4(positionIn.xyz, 1));
    */

//    gl_Position = projectionMatrix * vec4(positionIn.xyz, 1.0);

//    vec4 p = projectionMatrix * vec4(positionIn.xyz, 1.0);
    vec4 p = projectionMatrix * viewMatrix * modelMatrix * vec4( positionIn, 1.0);

//    p.z = 0;

    gl_Position = p;

    outPos = vec3(vec4(positionIn.xyz, 1));

}
