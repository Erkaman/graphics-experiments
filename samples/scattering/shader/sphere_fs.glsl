#include "shader_lib/scattering.glsl"

float logLuminance(vec4 c)
{
//	return 5.3f;
	return log(c.r * 0.2126f + c.g * 0.7152f + c.b * 0.0722f);
}



out vec4 fragmentColor;

in vec3 texCoord;

uniform float delta;

in vec3 worldSpacePosition;

uniform vec3 sunDirection;
uniform float reileighCoefficient;
uniform float mieCoefficient;
uniform float mieDirectionalG;
uniform float turbidity;

uniform vec3 cameraPosition;

void main()
{
    	float sunE = sunIntensity(dot(sunDirection, vec3(0.0f, 1.0f, 0.0f)));


	// extinction (absorbtion + out scattering)
	// rayleigh coefficients
	vec3 betaR = totalRayleigh(lambda) * reileighCoefficient;

	// mie coefficients
	vec3 betaM = totalMie(lambda, K, turbidity) * mieCoefficient;


	// optical length
	// cutoff angle at 90 to avoid singularity in next formula.
	float zenithAngle = acos(max(0, dot(up, normalize(worldSpacePosition - vec3(0, 0, 0)))));
	float sR = rayleighZenithLength / (cos(zenithAngle) + 0.15 * pow(93.885 - ((zenithAngle * 180.0f) / pi), -1.253));
	float sM = mieZenithLength / (cos(zenithAngle) + 0.15 * pow(93.885 - ((zenithAngle * 180.0f) / pi), -1.253));


	// combined extinction factor
	// see the bottom of page 10 in
	// http://amd-dev.wpengine.netdna-cdn.com/wordpress/media/2012/10/ATI-LightScattering.pdf
	// sR and sM is distance to camera.
	vec3 Fex = exp(-(betaR * sR + betaM * sM));


	// in scattering

	// TODO is the camera position in the right space????
	float cosTheta = dot(normalize(worldSpacePosition), sunDirection);

	float rPhase = rayleighPhase(cosTheta);
	// first equation
	//http://amd-dev.wpengine.netdna-cdn.com/wordpress/media/2012/10/ATI-LightScattering.pdf
	vec3 betaRTheta = betaR * rPhase;

	float mPhase = hgPhase(cosTheta, mieDirectionalG);
	// second equation
	// http://amd-dev.wpengine.netdna-cdn.com/wordpress/media/2012/10/ATI-LightScattering.pdf
	vec3 betaMTheta = betaM * mPhase;


	// third equation
	// http://amd-dev.wpengine.netdna-cdn.com/wordpress/media/2012/10/ATI-LightScattering.pdf
	vec3 Lin = sunE * ((betaRTheta + betaMTheta) / (betaR + betaM)) * (1.0f - Fex);

	// nightsky
/*	vec3 direction = normalize(worldSpacePosition - cameraPosition);
	float theta = acos(direction.y); // elevation --> y-axis, [-pi/2, pi/2]
	float phi = atan(direction.z, direction.x); // azimuth --> x-axis [-pi/2, pi/2]
	vec2 uv = vec2(phi, theta) / vec2(2*pi, pi) + vec2(0.5f, 0.0f);
	vec3 L0 = texture(sDiffuse, uv).rgb * Fex;*/

	// input color
	vec3 L0 = vec3(0,0,0);

	// composition + solar disc
//	if (cosTheta > sunAngularDiameterCos)
//		L0 += sunE * Fex;

	vec4 fragmentColor0 = vec4(L0 + Lin, 1);
	fragmentColor0.w = logLuminance(fragmentColor0);

	fragmentColor = fragmentColor0;

	fragmentColor = fragmentColor0;
}
