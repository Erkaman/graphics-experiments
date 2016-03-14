out vec4 fragmentColor;

in vec2 texCoord;

uniform sampler2D tex;



// it's as if tex is not loaded

void main(){


    fragmentColor = texture(tex,texCoord);
//    fragmentColor = vec4(texCoord,0,1);

}
