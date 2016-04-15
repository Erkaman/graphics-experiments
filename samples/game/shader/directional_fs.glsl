#include "shader/lighting_lib.glsl"

in vec2 texCoord;

out vec4 fragmentColor;

uniform sampler2D colorTexture;
uniform sampler2D depthTexture;
uniform sampler2D normalTexture;
uniform sampler2D specularTexture;
uniform sampler2DShadow shadowMap;
uniform sampler2D lightGrid;
uniform sampler2D lightIndexTexture;

uniform vec2 screenSize;
uniform float gridCountRcp;
uniform float lightIndexTextureSize;

uniform vec3 inAmbientLight;
uniform vec3 inSceneLight;
uniform vec3 eyePos;
uniform vec3 viewSpaceLightDirection;
uniform mat4 lightVp;
uniform mat4 lightVpTimesInverseViewMatrix;
uniform mat4 inverseViewMatrix;
uniform mat4 toViewSpacePositionMat;

uniform vec4 lightDirection;
uniform mat4 proj;
uniform mat4 inverseViewNormalMatrix;
uniform mat4 invViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

uniform mat4 paraboloidBasis;
uniform mat4 basis;


uniform vec3 pointLightPosition[256];
uniform float pointLightRadius[256];
uniform vec3 pointLightColor[256];

uniform sampler2D envMapFront;
uniform sampler2D envMapBack;


bool isInt(float f, float eps) {
    return
	((f - floor(f)) < eps ||
	 (ceil(f) - f) < eps);
}

void main() {



    vec3 specColor;
    float specShiny;


    vec3 diffColor;
    float ao;

    readColorTexture(colorTexture, texCoord, diffColor, ao, specColor, screenSize.x, screenSize.y);


    vec3 n;
    float id;
    readNormalTexture(normalTexture, texCoord, n, id, specShiny);


    vec3 specMat = specColor;

    vec3 envMapSample = vec3(0);

    vec3 viewSpacePosition = getViewSpacePosition(toViewSpacePositionMat, depthTexture, texCoord);

    vec3 v = -(viewSpacePosition);
    vec3 l= -viewSpaceLightDirection;

    if(id == 1.0) { // if car

	// add fresnel.
	specMat += (vec3(1.0) - specColor)  * pow(clamp(1.0 + dot(-v, n), 0.0, 1.0), 5.0);

	vec3 reflectionVector = (inverseViewNormalMatrix *
				 vec4(
				     reflect(-v, n.xyz ), 0.0)).xyz;

//	envMapSample = texture(envMap, reflectionVector).rgb;

	vec3 R  = normalize((paraboloidBasis *  vec4(reflectionVector, 0.0))).xyz;

//	vec3 R  = (reflectionVector).xyz;


	vec2 front;
	front.x = ((R.x / (2*(1 + R.z))) + 0.5);
	front.y = 1-((R.y / (2*(1 + R.z))) + 0.5);

	vec2 back;
	back.x = 1.0-((R.x / (2*(1 - R.z))) + 0.5);
	back.y = 1-((R.y / (2*(1 - R.z))) + 0.5);

	vec4 forward = texture( envMapFront, front );
	vec4 backward = texture( envMapBack, back );

	// flip front guy.
	envMapSample = max(forward, backward).xyz;
//	envMapSample = forward.xyz;
//	envMapSample = backward.xyz;

//	envMapSample = reflectionVector.z > 0 ? forward.xyz : backward.xyz;




    }

    vec3 ambientLight = inAmbientLight;
    vec3 sceneLight = inSceneLight;




    float aoOnly =0.0;

    vec4 shadowCoord = mulWhereWIsOne(lightVpTimesInverseViewMatrix, viewSpacePosition.xyz);


    if(id == 2.0) {

	waterShader(viewSpacePosition, proj, specColor, invViewMatrix, eyePos, diffColor, specMat, sceneLight, specShiny, envMapSample, ambientLight);


    }

    /*
      Compute directional lighting.
     */


    float diff=  calcDiff(l,n);
    float spec= calcSpec(l,n,v);
    float visibility = calcVisibility(shadowMap, diff, shadowCoord);

//    specShiny = 0.0;
//    specColor = vec3(0);
//    spec = 0.0;

    fragmentColor =vec4(vec3(1.0-ao), 1.0) * aoOnly +
	(1.0 - aoOnly)*calcLighting(
	    ambientLight,
	    sceneLight,
	    specShiny,
	    diffColor,
	    specMat,
	    diff,
	    spec,
	    visibility,
	    envMapSample );

    if(id == 1.0) { // if car

	fragmentColor = vec4(envMapSample, 1.0);
    }

//    fragmentColor = vec4(vec3(diff),1);


    /*
    if(id == 1.0) { // if car
	vec3 reflectionVector = (inverseViewNormalMatrix *
				 vec4(
				     reflect(-v, n.xyz ), 0.0)).xyz;

	envMapSample = texture(envMap, reflectionVector).rgb;

	fragmentColor = vec4(envMapSample, 1.0);
    }
    */

#ifdef IS_TILED

    float GRID_COUNT = 10.0;

    float gridCellSize = gridCountRcp;


//    vec3 gridColor = texture(lightGrid, texCoord  ).y > 0 ? vec3(1,0,0) : vec3(0,0,0);


    vec3 sample = texture(lightGrid, texCoord  ).rgb;
    float offset = sample.r;
    float count = sample.g;

    ambientLight = vec3(0.75);

    sceneLight = vec3(1);


    for(int i = 0; i < count; ++i) {

	//	offset = 0;

	vec2 lightIndexCoord = vec2(
	    mod( (offset + i), lightIndexTextureSize),
	    ((offset + i) / lightIndexTextureSize)
	    );

	lightIndexCoord *= (1.0 / lightIndexTextureSize);

	int pointLightIndex = int(texture(lightIndexTexture, lightIndexCoord).r);

	//fragmentColor = vec4(, 1.0);
//	fragmentColor = vec4(pointLightPosition[sample], 1.0);


	vec3  pointLightColor = pointLightColor[pointLightIndex];
	vec3  pointLightPosition = pointLightPosition[pointLightIndex];
	float pointLightRadius = pointLightRadius[pointLightIndex];

	vec3 lightCenter =  (viewMatrix *  vec4(pointLightPosition,1)  ).xyz;

    vec3 lightDist = viewSpacePosition - lightCenter;
    vec3 l = -normalize(lightDist);

    float diff=  calcDiff(l,n);

//    float spec = 0;
    float spec= calcSpec(l,n,v);

        float ztest = step(0, pointLightRadius - length(lightDist));


	    vec4 light = calcLighting(
	ambientLight,
	sceneLight,
	    specShiny,
	    diffColor,
	    specMat,
	    diff,
	    spec,
	    visibility,
	    vec3(0) );

    float atten = clamp(1.0 - length(lightDist) / pointLightRadius, 0,1);
    fragmentColor.xyz += light.xyz * vec3(pointLightColor) * atten;


//    fragmentColor.xyz += vec3(1,0,0);

    }


#endif




/*
    if(

	isInt(   texCoord.x / gridCellSize, 0.015  ) ||
	isInt(   texCoord.y / gridCellSize, 0.015  )


	){
	fragmentColor = vec4(vec3(0,1,0), 1.0);

    }
*/
}
