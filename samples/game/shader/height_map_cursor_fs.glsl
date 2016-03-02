out vec4 fragmentColor;

in float isRender;
in float d;

out vec4 geoData[3];


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
    geoData[0] = vec4(vec3(1,1,1),1);
    geoData[1] = vec4(
	normalize(-vec3(-0.705072f, -0.958142f, -0.705072f)),0);

}
