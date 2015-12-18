out vec4 fragmentColor;

in vec2 texCoord;

uniform sampler2D tex;

// it's as if tex is not loaded

void main(){
    vec4 sample = texture(tex,texCoord);


    if(sample.a < 0.75)
	discard;

    fragmentColor = sample;
}
