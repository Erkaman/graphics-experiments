out vec4 fragmentColor;

void main(){
    fragmentColor = vec4(vec3(gl_FragCoord.z),1 );

}
