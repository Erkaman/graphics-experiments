out vec3 fragmentColor;

uniform float id;

void main(){
    fragmentColor = vec3( float(id+1),
			  float(0),

			  float(0) );

}
