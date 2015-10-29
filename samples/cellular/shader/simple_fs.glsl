
uniform sampler2D tex;

in vec3 lightVec;
in vec3 halfVec;
in vec2 v_texcoord;


uniform vec3 spec_color;


uniform float materialShininess;

#ifdef NORMAL_MAPPING
	uniform sampler2D normalMap;
#else
	in vec3 v_normal ;
#endif

#ifdef SPEC_MAPPING
	uniform sampler2D specularMap;
#endif

out vec4 fragmentColor;


// illum is shininess.
// Ks is specular color.

void main(void) {

	vec3 ambientComponent;
	vec3 diffuseComponent= vec3(0.0);
	vec3 specularComponent= vec3(0.0);

	float lamberFactor =0.0;
	float specularFactor=0.0;

	vec3 bump ;

	vec4 colorSample  = texture(tex, v_texcoord) ;
	vec3 color = vec3(1,1,1); //colorSample.rgb;
	float alpha = colorSample.a ;

#ifdef NORMAL_MAPPING
		bump		=  texture(normalMap, v_texcoord).rgb * 2.0 - 1.0;
		lamberFactor  =  max(0.0,dot(lightVec, bump) );
		specularFactor = max(0.0,pow(dot(halfVec,bump),materialShininess)) ;

#else
		lamberFactor  =  max(0.0,dot(lightVec, v_normal) );
		specularFactor = max(0.0,pow(dot(halfVec,v_normal),materialShininess)) ;

#endif

#ifdef SPEC_MAPPING
		vec3 matTextColor = texture(specularMap, v_texcoord).rgb;
#else
		vec3 matTextColor = spec_color;
#endif

       vec3 lightColorAmbient = vec3(0.5);
       vec3 lightColorDiffuse = vec3(0.4);
       vec3 lightColorSpecular = vec3(1);

	ambientComponent  = lightColorAmbient  * color ;
	diffuseComponent  = lightColorDiffuse  * color *  lamberFactor;
	specularComponent = lightColorSpecular * matTextColor * specularFactor;

//	fragmentColor = vec4(vec3(v_normal), 1.0);

//	fragmentColor = vec4(vec3(specularFactor), 1.0);


	fragmentColor = vec4(
	    ambientComponent + (/*diffuseComponent + */specularComponent) ,colorSample.a)  ;
}
