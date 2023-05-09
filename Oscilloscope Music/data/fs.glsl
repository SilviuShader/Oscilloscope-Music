#version 330 core

#define SQRT2 1.4142135623730951

in vec3 fragXYLength;

uniform float uIntensity;
uniform float uSize;
uniform float uIntensityBase;

out vec4 finalColor;

float erf(float x) 
{
	float s = sign(x), a = abs(x);
	x = 1.0 + (0.278393 + (0.230389 + (0.000972 + 0.078108 * a) * a) * a) * a;
	x *= x;
	return s - s / (x * x);
}

void main()
{
	float len = fragXYLength.z;
	vec2 xy = fragXYLength.xy;

	float alpha;
	float sigma = uSize / (2.0+2.0*1000.0*uSize/50.0);

	alpha = erf(xy.x/SQRT2/sigma) - erf((xy.x-len)/SQRT2/sigma);
	alpha *= exp(-xy.y*xy.y/(2.0*sigma*sigma))/2.0/len*uSize;

	vec3 rgb = vec3(0.0, 1.0, 0.0);

	alpha = pow(alpha,1.0-uIntensityBase)*(0.01+min(0.99,uIntensity*3.0));
	finalColor = vec4(rgb, alpha);
}