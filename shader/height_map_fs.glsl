out vec4 fragmentColor;

//uniform vec4 color;

//in vec2 texCoord;

//uniform sampler2D tex;

uniform vec3 viewSpaceLightPosition;


in vec3 viewSpaceNormal;
in vec3 viewSpacePosition;
vec3 scene_ambient_light = vec3(0.05, 0.05, 0.05);

vec3 calculateAmbient(vec3 ambientLight, vec3 materialAmbient)
{
    return /*ambientLight */ materialAmbient;
}

vec3 sampleDiffuseTexture() {
    return vec3(0.33, 0.33, 0.33);
}

void main()
{
    // this is our height map color.
    vec3 ambient = sampleDiffuseTexture();


    vec3 diffuse = sampleDiffuseTexture();

    vec3 normal = normalize(viewSpaceNormal);
    vec3 directionToLight =
	normalize(viewSpaceLightPosition - viewSpacePosition);

    vec3 shading = calculateAmbient(scene_ambient_light, ambient);
	//+ calculateDiffuse(scene_light, diffuse, normal, directionToLight);

//    vec3 shading = calculateAmbient(scene_ambient_light, ambient);
    fragmentColor = vec4(shading, 1.0);
}