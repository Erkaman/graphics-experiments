out vec3 fragmentColor;

in float id;

void main()
{
    fragmentColor = vec3( float(0),
//			  float((id+1.0)/30000.0),
			  float(id+1.0),

			  float(0) );
}
