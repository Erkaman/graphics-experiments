out vec4 fragmentColor;

uniform vec3 viewSpaceLightPosition;

in vec3 vertexColor;

vec3 material_specular_color = vec3(1.0);
float material_shininess = 25.0;

in vec3 viewSpaceNormal;
in vec3 viewSpacePosition;
vec3 scene_ambient_light = vec3(0.1);
vec3 scene_light = vec3(0.8, 0.8, 0.8);
vec3 calculateAmbient(vec3 ambientLight, vec3 materialAmbient)
{
    return ambientLight *  materialAmbient;
}

vec3 calculateDiffuse(vec3 diffuseLight, vec3 materialDiffuse,
 vec3 normal, vec3 directionToLight) {
 return diffuseLight * materialDiffuse * dot(normal, directionToLight);
}

vec3 calculateSpecular(vec3 specularLight, vec3 materialSpecular,
 float materialShininess, vec3 normal,
vec3 directionToLight, vec3 directionFromEye)
{
 vec3 h = normalize(directionToLight - directionFromEye);
 return specularLight * materialSpecular
 * pow(max(0, dot(h, normal)), materialShininess);
}

vec3 sampleDiffuseTexture() {
    return vec3(0.5, 0.5, 0.5);
}

void main()
{
    // this is our height map color.
    vec3 ambient = sampleDiffuseTexture();


    vec3 diffuse = sampleDiffuseTexture();

    vec3 normal = normalize(viewSpaceNormal);
    vec3 directionToLight =
	normalize(viewSpaceLightPosition - viewSpacePosition);

    vec3 specular = material_specular_color;

    vec3 directionFromEye = normalize(viewSpacePosition);

    vec3 shading = //calculateAmbient(scene_ambient_light, ambient)
	calculateDiffuse(scene_light, diffuse, normal, directionToLight);
/*	calculateSpecular(scene_light, specular, material_shininess,
	normal, directionToLight, directionFromEye);*/

    fragmentColor = vec4(clamp(shading, 0, 1), 1.0);
}
