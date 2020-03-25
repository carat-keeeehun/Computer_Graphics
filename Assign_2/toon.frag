#version 140
#extension GL_ARB_compatibility: enable

//in vec4 p;
//in vec3 normal, lightDir;

in vec3 fragN, fragL;

uniform float Num_range;

void main()
{	
	vec4 color;
	
	// ToDo

	vec3 N = normalize(fragN);
	vec3 L = normalize(fragL);

	float angle = abs(acos(dot(N, L))) * 180.0 / 3.14;

	for(int i=0; i<int(Num_range); i++){
		float a1 = float(i) * 90.0 / Num_range;
		float a2 = float(i+1) * 90.0 / Num_range;

		if(angle > a1 && angle < a2)
			gl_FragColor = vec4((Num_range-float(i))/Num_range, (Num_range-float(i))/Num_range, (Num_range-float(i))/Num_range, 1.0);
	}
}