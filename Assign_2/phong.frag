#version 140
#extension GL_ARB_compatibility: enable

// Normal, Eye, Light
in vec3 fragN, fragE, fragL;
out vec4 fragColor;

uniform vec4 Ambient, Diffuse, Specular;
uniform vec4 LightPosition;
uniform float Alpha;

void main()
{	
	vec4 color;

	vec3 N = normalize(fragN);
	vec3 L = normalize(fragL);
	vec3 H = normalize(fragL+fragE);

	vec4 ambient = Ambient;

	vec4 diffuse = max(dot(N,L), 0.0) * Diffuse;

	vec4 specular = pow(max(dot(N,H), 0.0), Alpha) * Specular;

	// Check whether dot(N,L) is positive.
	if(dot(N,L) < 0.0)
		specular = vec4(0.0, 0.0, 0.0, 1.0);

	fragColor = ambient + diffuse + specular;
	fragColor.a = 1.0;
}