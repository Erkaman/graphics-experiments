
/*
  l: light vector. vector FROM point TO light.
  n: normal

  and do make sure both are in the same space!
 */
float calcDiff(vec3 l, vec3 n) {
    return clamp(dot(l,n),0,1);
}

/*
  l: light vector. vector FROM point TO light.
  n: normal
  v: view vector. vector FROM point TO camera.

  and do make sure both are in the same space!
 */
float calcSpec(vec3 l, vec3 n, vec3 v) {
    return clamp(dot(normalize(l+v),n),0,1);
}

vec4 calcLighting(
    vec3 ambientLight, // ambient material

    vec3 sceneLight, // diffuse material

    float specShiny,

    vec3 diffColor, // diffuse color from texture
    vec3 specColor, // specular from texture

    float diff, // phong diff
    float spec, // phong spec.

    float visibility, // shadowing
    vec3 envMapSample
    ) {
    vec3 finalcolor=ambientLight*diffColor; // ambient

    finalcolor.xyz+=(
	diffColor*sceneLight*diff * visibility  +
	specColor*pow(spec,specShiny) * visibility);

    finalcolor.xyz += specColor * spec * envMapSample * 0.4;

    return vec4(finalcolor,1.0);

//    return vec4(specColor * spec * envMapSample * 0.4, 1.0);
}

float rand(vec4 seed) {
    float dot_product = dot(seed, vec4(12.9898,78.233,45.164,94.673));
    return fract(sin(dot_product) * 43758.5453);
}

vec2 poissonDisk[4] = vec2[](
/*
vec2(-0.640363, 0.639219),
vec2(-0.438874, 0.305672),
vec2(-0.283887, 0.773152),
vec2(-0.943998, 0.093681),
vec2(-0.0211312, 0.0981202),
vec2(0.336444, 0.911633),
vec2(0.244629, 0.519954),
vec2(0.637154, 0.537039),
vec2(0.445192, -0.173428),
vec2(-0.381318, -0.139341),
vec2(0.460371, 0.193586),
vec2(-0.766582, -0.283754),
vec2(0.932469, 0.0815573),
vec2(0.634411, -0.5472),
vec2(0.157984, -0.731492),
vec2(-0.386095, -0.581438)
*/

   vec2( -0.94201624, -0.39906216 ),
   vec2( 0.94558609, -0.76890725 ),
   vec2( -0.094184101, -0.92938870 ),
   vec2( 0.34495938, 0.29387760 )
 );

// calc shadows.
float calcVisibility(sampler2DShadow shadowMap, float diff, vec4 shadowCoord ) {
    float cosTheta = diff;
    float bias = 0.001; //*tan(acos(cosTheta))+0.0005;

    float visibility = 1.0;

#if defined HIGH_QUALITY

    for (int i=0;i<4;i++){

	int index = i;

	vec2 sample = reflect(poissonDisk[index], normalize(gl_FragCoord.xy) );

	 visibility -=
	     0.25 * (1.0-texture( shadowMap, vec3(shadowCoord.xy+sample/3000.0, ( (shadowCoord.z-bias) / shadowCoord.w) )  ));
     }

#else

    visibility -= 1.0* (1.0- texture(shadowMap, vec3(shadowCoord.xy, ( (shadowCoord.z-bias) / shadowCoord.w )  )));
#endif

    return visibility;
}

//
/*
float computeAo() {


    // circle center
    vec3 c = vec3(0,0,1.13129);
    float R = 11.502;
    vec3 n = normalOut;
    vec3 p0 = positionOut;

//    float d = distance(positionOut,c);



    float ro = dot(n,c) - dot(n, p0);
    float h = R - ro;

    // off center distance
    float d = distance(p0,  c + ro * n );

    float vcap = (1.0/3.0) * 3.14 * h*h * (3*R - h);

    float f = (h*h) / (h*h + d*d);

    float a = 0.0006;
    return exp(-1 * a * f * vcap);

//    return vcap / (0.5 * 1.33333 * 3.14 * R*R*R );
}
*/


/*
float toLinearDepth(float depth) {
    float near = 0.1;
    float far = 500.0;

    return 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
}
*/

vec3 getViewSpacePosition(mat4 invProj, sampler2D depthTexture, vec2 texCoord) {

    float x = texCoord.x * 2 - 1;
    float y = (texCoord.y) * 2 - 1;
//    float z = toLinearDepth(texture(depthTexture, texCoord).r* 2 - 1);
    float z = texture(depthTexture, texCoord).r * 2 - 1;
    vec4 projectedPos = vec4(x, y,z

			     , 1.0f);

    vec4 p =  invProj * projectedPos;

    return p.xyz / p.w;
}

void readNormalTexture(
    sampler2D normalTexture, vec2 texCoord, out vec3 n, out float id) {

    vec4 sample = texture(normalTexture, texCoord);
    vec3 viewSpaceNormal = sample.xyz;

    n = viewSpaceNormal;
    id = sample.w;
}

void readSpecularTexture(sampler2D specularTexture, vec2 texCoord, out vec3 specColor, out float specShiny) {

    vec4 sample = texture(specularTexture, texCoord);
    specColor = sample.xyz;
    specShiny = sample.w * 100;
}

void readColorTexture(sampler2D colorTexture, vec2 texCoord, out vec3 diffColor, out float ao) {

    vec4 sample = texture(colorTexture, texCoord);
    diffColor = sample.xyz;
    ao = sample.w;
}

void waterShader(vec3 viewSpacePosition, mat4 proj, vec3 specColor, sampler2D refractionMap, sampler2D reflectionMap,
		 mat4 invViewMatrix, vec3 eyePos, inout vec3 diffColor, inout vec3 specMat, inout vec3 sceneLight, inout float specShiny, inout vec3 envMapSample, inout vec3 ambientLight) {
/*	vec4 clipSpace = proj * vec4( viewSpacePosition, 1.0);


	vec2 ndc = clipSpace.xy / clipSpace.w;
	ndc = ndc * 0.5 + 0.5;

	vec2 refractionTexcoord = ndc;
	vec2 reflectionTexcoord = vec2(ndc.x, 1 -ndc.y);


	vec2 distort = specColor.xy;

	refractionTexcoord += distort;
	reflectionTexcoord += distort;

	refractionTexcoord = clamp(refractionTexcoord, 0.001, 1.0 - 0.001);


	vec3 refraction = texture(refractionMap, refractionTexcoord).xyz;
	vec3 reflection = texture(reflectionMap, reflectionTexcoord).xyz;
*/

	vec3 refraction = diffColor.xyz;
	vec3 reflection = specColor.xyz;



	/*
	vec3 refraction = vec3(1,0,0);
	vec3 reflection = vec3(0,1,0);*/


	vec3 worldPosition = (invViewMatrix * vec4(viewSpacePosition, 1)).xyz;

	vec3 toCameraVector = normalize(eyePos - worldPosition.xyz);

	float fresnel = dot(
	    toCameraVector, vec3(0,1,0));

	diffColor = mix(refraction, 0.4 * reflection, 1.0 - fresnel);

	specMat = 0.6 * sceneLight;

	specShiny = 20.0;
	envMapSample = vec3(0,0,0);

	ambientLight = vec3(1,1,1);
//	sceneLight = vec3(1);
//	sceneLight = vec3(0);

}
