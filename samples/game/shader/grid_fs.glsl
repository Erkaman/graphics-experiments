out vec4 fragmentColor;

in vec2 pos;
in vec3 color;

uniform sampler2D tex;

uniform float gridSize;

int isInt(float f) {
    float eps = 0.1;

    if((f - floor(f)) < eps ||
       (ceil(f) - f) < eps) {
	return 1;
    } else {
	return 0;
    }
}

void main() {

    vec2 texCoord = 0.5 + 0.5 * pos;
    texCoord.y = 1.0 - texCoord.y;
    fragmentColor = vec4( texture(tex, texCoord).xyz ,1);

    vec2 ints = (pos + 1) * (1 / gridSize);

    if(isInt(ints.x)==1 &&  isInt(ints.y)==1  ) {
	fragmentColor = vec4(vec3(1,1,0), 1);
    } else {
	fragmentColor = vec4( texture(tex, texCoord).xyz ,1);

//	fragmentColor = vec4(color, 1);
    }

//    fragmentColor = vec4(color, 1);

}
