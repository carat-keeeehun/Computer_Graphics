#version 140
//#define GL_compatibility_profile 1
#extension GL_ARB_compatibility: enable

out vec3 fragN, fragE, fragL;

uniform vec4 LightPosition;

void main()
{	
	fragN = normalize(gl_NormalMatrix * gl_Normal);
	fragE = gl_Vertex.xyz;
	fragL = LightPosition.xyz - gl_Vertex.xyz;

	if(LightPosition.w != 0.0)
		fragL = LightPosition.xyz - gl_Vertex.xyz;

	gl_Position = gl_ModelViewProjectionMatrix * (gl_Vertex);
}
