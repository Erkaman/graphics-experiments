
uniform sampler2D diffMap;

out vec4 fragmentColor;


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


#ifdef NORMAL_MAPPING
uniform sampler2D heightMap;

/*
vec2 parallaxMapping(vec3 eyeVec, vec2 texcoord)
{
    vec3 V = normalize(eyeVec);
    vec2 T = texcoord;

    float initialHeight = texture(normalMap, T).a;

   // calculate amount of offset for Parallax Mapping With Offset Limiting
   vec2 texCoordOffset = 0.1 * V.xy * initialHeight;


   return texcoord - texCoordOffset;
}*/



// steep parallax mapping
/*
vec2 parallaxMapping(vec3 eyeVec, vec2 texcoord) {

    vec3 V = eyeVec;
    vec2 T = texcoord;


     // determine number of layers from angle between V and N
   const float minLayers = 40;
   const float maxLayers = 100;
   float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0, 0, 1), V)));

   // height of each layer
   float layerHeight = 1.0 / numLayers;
   // depth of current layer
   float currentLayerHeight = 0;
   // shift of texture coordinates for each iteration
   vec2 dtex = 0.01 * V.xy / V.z / numLayers;

   // current texture coordinates
   vec2 currentTextureCoords = T;

   // get first depth from heightmap
   float heightFromTexture = texture(heightMap, currentTextureCoords).r;

   // while point is above surface
   while(heightFromTexture > currentLayerHeight)
   {
      // to the next layer
      currentLayerHeight += layerHeight;
      // shift texture coordinates along vector V
      currentTextureCoords -= dtex;
      // get new depth from heightmap
      heightFromTexture = texture(heightMap, currentTextureCoords).r;
   }

   return currentTextureCoords;

}
*/


/*
//relief parallax mapping:
vec2 parallaxMapping(vec3 eyeVec, vec2 texcoord)
{
    vec3 V = normalize(eyeVec);
    vec2 T = texcoord;

   // determine required number of layers
   const float minLayers = 10;
   const float maxLayers = 15;
//   float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0, 0, 1), V)));
   float numLayers = 150;

   // height of each layer
   float layerHeight = 1.0 / numLayers;
   // depth of current layer
   float currentLayerHeight = 0;
   // shift of texture coordinates for each iteration
   vec2 dtex = 0.1 * V.xz / V.y / numLayers;

//   float a = abs(0.03 *V.y / V.z);
//   fragmentColor = vec4(a, 0 , 0, 1);
//   fragmentColor = vec4(abs(V.z),0 , 0, 1);

   // current texture coordinates
   vec2 currentTextureCoords = T;

   // depth from heightmaps
   float heightFromTexture = texture(normalMap, currentTextureCoords).w;

   // while point is above surface
   while(heightFromTexture > currentLayerHeight)
   {
      // go to the next layer
      currentLayerHeight += layerHeight;
      // shift texture coordinates along V
      currentTextureCoords -= dtex;
      // new depth from heightmap
      heightFromTexture = texture(normalMap, currentTextureCoords).w;
   }

   ///////////////////////////////////////////////////////////
   // Start of Relief Parallax Mapping

   // decrease shift and height of layer by half
   vec2 deltaTexCoord = dtex / 2;
   float deltaHeight = layerHeight / 2;

   // return to the mid point of previous layer
   currentTextureCoords += deltaTexCoord;
   currentLayerHeight -= deltaHeight;

   // binary search to increase precision of Steep Paralax Mapping
   const int numSearches = 5;
   for(int i=0; i<numSearches; i++)
   {
      // decrease shift and height of layer by half
      deltaTexCoord /= 2;
      deltaHeight /= 2;

      // new depth from heightmap
      heightFromTexture = texture(normalMap, currentTextureCoords).w;

      // shift along or agains vector V
      if(heightFromTexture > currentLayerHeight) // below the surface
      {
         currentTextureCoords -= deltaTexCoord;
         currentLayerHeight += deltaHeight;
      }
      else // above the surface
      {
         currentTextureCoords += deltaTexCoord;
         currentLayerHeight -= deltaHeight;
      }
   }

   // return results
   // parallaxHeight = currentLayerHeight;    return currentTextureCoords;

   return currentTextureCoords;

   }

*/

float ray_intersect_rm(sampler2D reliefmap, vec2 dp, vec2 ds) {

    const int linear_search_steps=10;
    const int binary_search_steps=5;
    float depth_step=1.0/linear_search_steps;
    float size=depth_step; // current size of search window
    float depth=0.0; // current depth position
// best match found (starts with last position 1.0)
    float best_depth=1.0;
// search from front to back for first point inside the object
    for ( int i=0; i< linear_search_steps-1;i++){
	depth+=size;
	vec4 t=texture(reliefmap,dp+ds*depth);
	if (best_depth>0.996) // if no depth found yet
	    if (depth >= t.w)
		best_depth=depth; // store best depth
    }
    depth=best_depth;
// search around first point (depth) for closest match
    for ( int i=0; i<binary_search_steps;i++) {
	size*=0.5;
	vec4 t=texture(reliefmap,dp+ds*depth);
	if (depth >=  t.w) {
	    best_depth = depth;
	    depth -= 2*size;}
	depth+=size;
    }
    return best_depth;
}

#endif

void main(void) {

    // texture coordinates.

#ifdef NORMAL_MAPPING

    vec3 V = normalize(eyeVecOut);

    // may need tweaking?
    float depth = 0.3;

    vec2 ds = V.xz*depth/V.y;
    vec2 dp = texcoordOut;

//    vec2 dtex = 0.1 * V.xz / V.y / numLayers;

    float d =ray_intersect_rm(normalMap,dp,ds);


    vec2 T = dp+ds*d;

//    vec2 T = parallaxMapping(eyeVecOut, vec2(texcoordOut.x, 1-texcoordOut.y  ));


    // we have d.
    // v = view vector in eye space.
/*    p += v*d*s.z;
    l=normalize(p-lightpos.xyz);
*/
    vec3 l = lightVecOut;
#else
    vec2 T = vec2(texcoordOut.x, 1-texcoordOut.y  );
    vec3 l = lightVecOut;
#endif

    vec3 ambientComponent;
    vec3 diffuseComponent= vec3(0.0);
    vec3 specComponent= vec3(0.0);

    float lamberFactor =0.0;
    float specFactor=0.0;

    vec3 bump ;

#ifdef HEIGHT_MAPPING
//    vec4 colorSample = vec4(vec3(texture(heightMap, T).r) , 1.0);
   vec4 colorSample  = texture(diffMap, T) ;
//  vec4 colorSample = vec4(1,0,0,1);
#else
    vec4 colorSample  = texture(diffMap, T) ;
#endif

    vec3 color = colorSample.rgb;


    float alpha = colorSample.a ;

#ifdef NORMAL_MAPPING
    bump		=normalize ( (texture(normalMap, T).rgb - 0.5) * 2.0);
    lamberFactor  =  max(0.0,dot(lightVecOut, bump) );

    vec3 r = reflect(l, bump);
    specFactor = 10*max(0.0,pow(dot(eyeVecOut,r),/*specShiny*/10)) ;
#else
    lamberFactor  =  max(0.0,dot(lightVecOut, normalOut) );

    vec3 r = reflect(l, normalOut);
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
//	specComponent ,colorSample.a)  ;
	ambientComponent + (diffuseComponent + specComponent ) ,colorSample.a)  ;

    //fragmentColor = vec4(d, 0,0,1);

/*
#ifdef NORMAL_MAPPING
    fragmentColor = vec4(vec3(texture(normalMap, T).a), 1.0);
#endif
*/

//    fragmentColor = vec4(normalize(eyeVecOut), 1.0 );

}
