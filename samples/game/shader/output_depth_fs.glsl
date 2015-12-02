out float fragmentColor;

void main(){
//    fragmentColor = vec4(vec3(gl_FragCoord.z),1 );

    fragmentColor = gl_FragCoord.z;

}
