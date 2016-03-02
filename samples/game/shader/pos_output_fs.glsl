out vec4 fragmentColor;
in vec3 outPos;

void main() {
    fragmentColor = vec4(outPos, 1);
//    fragmentColor = vec4(vec3(0,1,0), 1);

}
