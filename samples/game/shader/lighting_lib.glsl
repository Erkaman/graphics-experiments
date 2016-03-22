
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

    finalcolor+= (diffColor*sceneLight)* (diff * visibility);
    finalcolor += specColor*pow(spec,specShiny) * visibility;
    finalcolor += (specColor * envMapSample) * (spec  * 0.4);

    return vec4(finalcolor,1.0);
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

vec4 mulWhereWIsOne(mat4 m, vec3 v) {
    return v.x * m[0] + (v.y * m[1] + (v.z * m[2] + m[3]) );
}

//toViewSpacePositionMat = invProj * Trans(-1,-1,-1) * Scale(2,2,2)
vec3 getViewSpacePosition(
    mat4 toViewSpacePositionMat,
    sampler2D depthTexture, vec2 texCoord) {

    vec3 pos = vec3(texCoord.x, texCoord.y, texture(depthTexture, texCoord).r);
    vec4 p = mulWhereWIsOne(toViewSpacePositionMat, pos.xyz);

    return p.xyz / p.w;
}


vec2 encode (vec3 n)
{
    vec2 enc = normalize(n.xy) * (sqrt(-n.z*0.5+0.5));
    enc = enc*0.5+0.5;
    return enc;
}
vec3 decode (vec4 enc)
{
    vec4 nn = enc*vec4(2,2,0,0) + vec4(-1,-1,1,-1);
    float l = dot(nn.xyz,-nn.xyw);
    nn.z = l;
    nn.xy *= sqrt(l);
    return nn.xyz * 2 + vec3(0,0,-1);
}

vec4 packSpecularTexture(vec3 specColor, float specShiny) {
    return vec4(specColor.x, 0,0,specShiny / 100.0 );
}

vec4 packNormalTexture(vec3 n, float id) {
    return vec4(encode(n), 0.0, id);
}

void readNormalTexture(
    sampler2D normalTexture, vec2 texCoord, out vec3 n, out float id) {

    vec4 sample = texture(normalTexture, texCoord);
/*
    n.x = sample.x;
    n.y = sample.y;
    n.z = sample.z;
*/
    n.xyz = decode(vec4(sample.xy, 0,0 ) );

//    n.z = sqrt(1.0 - dot(sample.xy, sample.xy));

//    n = sample.xyz;


//    n = sample.xyz * 0.5 + 0.5;

//    n = sample.xyz * 2.0 - 1.0;


/*
    float X = sample.x * 2 - 1;
    float Y = sample.y * 2 - 1;

    n = vec3(
	2*X / (1 + X*X + Y*Y),
	2*Y / (1 + X*X + Y*Y),
        (-1 + X*X + Y*Y) / (1 + X*X + Y*Y)

	);
*/

//    n.z *= sample.z;



    id = sample.w;
}

void readSpecularTexture(sampler2D specularTexture, vec2 texCoord, out vec3 specColor, out float specShiny) {

    vec4 sample = texture(specularTexture, texCoord);
    specColor = vec3(sample.x);
    specShiny = sample.w * 100;
}

vec3 RGB2YCoCg(vec3 c){
    return vec3( 0.25*c.r+0.5*c.g+0.25*c.b, 0.5*c.r-0.5*c.b +0.5, -0.25*c.r+0.5*c.g-0.25*c.b +0.5);
}

vec3 YCoCg2RGB(vec3 c){
    c.y-=0.5;
    c.z-=0.5;
    return vec3(c.r+c.g-c.b, c.r + c.b, c.r - c.g - c.b);
}


vec4 packColorTexture(vec3 diffColor, float ao ) {

    vec3 fragColor = RGB2YCoCg(diffColor);

    vec2 crd = gl_FragCoord.xy;

    bool pattern = (mod(crd.x,2.0)==mod(crd.y,2.0));
    fragColor.g = (pattern)?fragColor.b: fragColor.g;
    fragColor.b = 0;


//    vec3 fragColor = diffColor;

     return vec4(
	 fragColor,
		     ao);

}

void readColorTexture(sampler2D colorTexture, vec2 texCoord, out vec3 diffColor, out float ao, float fboWidth, float fboHeight) {

//    float chroma = 0;

    vec2 crd = gl_FragCoord.xy;
    bool pattern = (mod(crd.x,2.0)==mod(crd.y,2.0));

    float chroma = texture(colorTexture, texCoord + vec2(1.0/fboWidth,0.0)).g;

    vec4 sample = texture(colorTexture, texCoord);
    vec3 col = sample.xyz;
    col.b=chroma;

    col.rgb = (pattern)?col.rbg:col.rgb;

    col.rgb = YCoCg2RGB(col.rgb);

//    diffColor = YCoCg2RGB(sample.xyz);
    diffColor = col.rgb;


    /*
    vec4 sample = texture(colorTexture, texCoord);

    diffColor = sample.rgb;

    ao = sample.w;
    */
}

void waterShader(vec3 viewSpacePosition, mat4 proj, vec3 specColor, sampler2D refractionMap, sampler2D reflectionMap,
		 mat4 invViewMatrix, vec3 eyePos, inout vec3 diffColor, inout vec3 specMat, inout vec3 sceneLight, inout float specShiny, inout vec3 envMapSample, inout vec3 ambientLight) {

    vec3 refraction = diffColor.xyz;
    vec3 reflection = specColor.xyz;

    vec3 worldPosition = (invViewMatrix * vec4(viewSpacePosition, 1)).xyz;

    vec3 toCameraVector = normalize(eyePos - worldPosition.xyz);

    float fresnel = dot(
	toCameraVector, vec3(0,1,0));

    diffColor = mix(refraction, 0.4 * reflection, 1.0 - fresnel);

    specMat = 0.6 * sceneLight;

    specShiny = 20.0;
    envMapSample = vec3(0,0,0);

    ambientLight = vec3(1,1,1);
}
