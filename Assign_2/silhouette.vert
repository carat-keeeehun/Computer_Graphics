#version 140
//#define GL_compatibility_profile 1
#extension GL_ARB_compatibility: enable

//out vec3 normal, lightDir;
//out vec4 p;

uniform float Thickness;

void main()
{	
    // ToDo
	vec4 vertex;
	vertex.xyzw = gl_Vertex.xyzw;

	vec3 normal = normalize(gl_NormalMatrix * gl_Normal);

	// Move each vertex along vertex.normal
	vertex.x = vertex.x + Thickness * normal.x;
	vertex.y = vertex.y + Thickness * normal.y;
	vertex.z = vertex.z + Thickness * normal.z;
	
	gl_Position = gl_ModelViewProjectionMatrix * (vertex);
}
