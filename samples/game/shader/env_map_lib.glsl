
vec4 envMapProject(float paraboloidDirection, vec3 viewSpacePosition, vec3 modelSpacePosition, vec3 carPos, float zfar, float znear) {

    gl_ClipDistance[0] = -1.0 * paraboloidDirection * dot(vec3(0,0,1), modelSpacePosition - carPos );

    vec4 outP = vec4(viewSpacePosition,1);
    outP = outP / outP.w;

    float L = length( outP.xyz );
    outP = outP / L;


    outP.z = outP.z + 1;
    outP.x = outP.x / outP.z;
    outP.y = outP.y / outP.z;


    outP.z = (L - znear)/(zfar-znear);
    outP.w = 1;

    return outP;
}
