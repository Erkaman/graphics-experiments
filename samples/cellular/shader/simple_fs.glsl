
uniform sampler2D diffMap;

in vec3 lightVecOut;
in vec3 eyeVecOut;
in vec2 texcoordOut;

uniform vec3 specColor;

uniform float specShiny;

#ifdef NORMAL_MAPPING
uniform sampler2D normalMap;
#else
in vec3 normalOut;
#endif

#ifdef SPEC_MAPPING
uniform sampler2D specMap;
#endif


#ifdef HEIGHT_MAPPING
uniform sampler2D heightMap;

vec2 parallaxMapping(vec3 eyeVec, vec2 texcoord) {

    vec3 V = eyeVec;

    float initialHeight = texture(heightMap, texcoord).r;

    vec2 texCoordOffset = 0.05 * V.xy * initialHeight;

    return texcoord -texCoordOffset;
//    return V.xy;
}

#endif

out vec4 fragmentColor;




void main(void) {

    // texture coordinates.

#ifdef HEIGHT_MAPPING
    vec2 T = parallaxMapping(eyeVecOut, vec2(texcoordOut.x, 1-texcoordOut.y  ));
#else
    vec2 T = vec2(texcoordOut.x, 1-texcoordOut.y  );
#endif

    vec3 ambientComponent;
    vec3 diffuseComponent= vec3(0.0);
    vec3 specComponent= vec3(0.0);

    float lamberFactor =0.0;
    float specFactor=0.0;

    vec3 bump ;

#ifdef HEIGHT_MAPPING
//    vec4 colorSample = vec4(vec3(texture(heightMap, T).r) , 1.0);
//  vec4 colorSample  = texture(diffMap, T) ;
  vec4 colorSample = vec4(1,0,0,1);
#else
    vec4 colorSample  = texture(diffMap, T) ;
#endif

    vec3 color = colorSample.rgb;


    float alpha = colorSample.a ;

#ifdef NORMAL_MAPPING
    bump		=  texture(normalMap, T).rgb * 2.0 - 1.0;
    lamberFactor  =  max(0.0,dot(lightVecOut, bump) );
    specFactor = max(0.0,pow(dot(eyeVecOut,bump),specShiny)) ;
#else
    lamberFactor  =  max(0.0,dot(lightVecOut, normalOut) );

    vec3 r = reflect(lightVecOut, normalOut);
    specFactor = max(0.0,pow(dot(eyeVecOut,r),specShiny)) ;
#endif


#ifdef SPEC_MAPPING
    vec3 materialSpec = texture(specMap, T).rgb;
#else
    vec3 materialSpec = specColor;
#endif

    vec3 lightColorAmbient = vec3(0.5);
    vec3 lightColorDiffuse = vec3(0.4);
    vec3 lightColorSpecular = vec3(0.15);

    ambientComponent  = lightColorAmbient  * color ;
    diffuseComponent  = lightColorDiffuse  * color *  lamberFactor;
    specComponent = lightColorSpecular * materialSpec * specFactor;

    fragmentColor = vec4(
	ambientComponent + (diffuseComponent + specComponent ) ,colorSample.a)  ;

}
