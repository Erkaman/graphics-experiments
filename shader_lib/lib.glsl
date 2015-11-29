
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

vec3 phongVertex(vec3 diffuseTextureSample, vec3 viewSpaceNormal, vec3 viewSpaceLightDirection, vec3 viewSpacePosition) {

    vec3 ambient = diffuseTextureSample;

    vec3 diffuse = diffuseTextureSample;

    vec3 normal = normalize(viewSpaceNormal);

//    vec3 directionToLight =
//	normalize(viewSpaceLightPosition - viewSpacePosition);
    vec3 directionToLight = viewSpaceLightDirection;

    vec3 directionFromEye = normalize(viewSpacePosition);

    return calculateAmbient(scene_ambient_light, ambient) +
	calculateDiffuse(scene_light, diffuse, normal, directionToLight);

}
