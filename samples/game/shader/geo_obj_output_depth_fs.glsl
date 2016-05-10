out vec4 fragmentColor;

uniform float diffMap;

out vec2 texcoordOut;
uniform sampler2DArray textureArray;

void main(){

    /*
    vec4 diffColor=texture(textureArray,vec3(texcoordOut,diffMap) );

    if(diffColor.a < 0.05)
	discard;
    */

    fragmentColor = vec4(vec3(gl_FragCoord.z),1 );
}
