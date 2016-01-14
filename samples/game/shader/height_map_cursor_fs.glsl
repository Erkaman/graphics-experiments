out vec4 fragmentColor;

in float isRender;
in float d;

void main()
{

/*
    if( isRender == 0)
	discard;

    vec3 col;

    if(d == 2) {
	col = vec3(1,0,0);
    } else if(d == 3) {
	col = vec3(0,1,0);
    }else if(d == 4) {
	col = vec3(0,0,1);
    }else {
	col = vec3(0,0,0);
    }
*/
    fragmentColor = vec4(vec3(1,0,0),1);

}
