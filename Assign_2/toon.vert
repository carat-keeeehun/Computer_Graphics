#version 140
//#define GL_compatibility_profile 1
#extension GL_ARB_compatibility: enable

//out vec3 normal, lightDir;
//out vec4 p;

out vec3 fragN;
out vec3 fragL;

uniform vec4 LightPosition;

void main()
{	
    // ToDo

	fragN = normalize(gl_NormalMatrix * gl_Normal);
	fragL = LightPosition.xyz - gl_Vertex.xyz;
	
	gl_Position = gl_ModelViewProjectionMatrix * (gl_Vertex);
}