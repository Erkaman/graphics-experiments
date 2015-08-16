
vec3 material_specular_color = vec3(1.0);
float material_shininess = 25.0;


vec3 scene_ambient_light = vec3(0.6);
vec3 scene_light = vec3(0.8);

vec3 calculateAmbient(vec3 ambientLight, vec3 materialAmbient)
{
    return ambientLight *  materialAmbient;
}

vec3 calculateDiffuse(vec3 diffuseLight, vec3 materialDiffuse,
 vec3 normal, vec3 directionToLight) {
    return diffuseLight * materialDiffuse * max(0,dot(normal, directionToLight));
}

vec3 phongVertex(vec3 diffuseTextureSample, vec3 viewSpaceNormal, vec3 viewSpaceLightPosition, vec3 viewSpacePosition) {

    vec3 ambient = diffuseTextureSample;

    vec3 diffuse = diffuseTextureSample;

    vec3 normal = normalize(viewSpaceNormal);

    vec3 directionToLight =
	normalize(viewSpaceLightPosition - viewSpacePosition);

    vec3 directionFromEye = normalize(viewSpacePosition);

    float dist = length(viewSpaceLightPosition - viewSpacePosition);

    return calculateAmbient(scene_ambient_light, ambient)  +
	calculateDiffuse(scene_light, diffuse, normal, directionToLight) * (1 - dist * dist);

//    return vec3(1 - dist * dist);
}



















out vec4 fragmentColor;

uniform vec3 viewSpaceLightPosition;

//in vec3 vertexColor;
in vec2 texCoord;

in vec3 viewSpaceNormal;
in vec3 viewSpacePosition;

uniform sampler2D grass;

void main()
{
    vec4 sample = texture(grass, texCoord);//vec4(0,1,0, texture(grass, texCoord).a);
    vec3 shading = phongVertex(sample.rgb, viewSpaceNormal, viewSpaceLightPosition, viewSpacePosition);
    fragmentColor = /*vec4(viewSpaceNormal,sample.a);*/ vec4( shading, sample.a);
}
