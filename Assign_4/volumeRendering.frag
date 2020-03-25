#version 140
#extension GL_ARB_compatibility: enable

in vec3 pixelPosition;
uniform vec3 eyePosition;
uniform vec3 objectMin;
uniform vec3 objectMax;
uniform vec3 up;
uniform sampler3D tex;
uniform sampler1D transferFunction;

#define MIP

void main(){
    vec3 right=normalize(cross(-eyePosition,up));
    vec3 virtualScreenCoord=eyePosition*0.5+
                                right*pixelPosition.x+
                                up*pixelPosition.y;

	// Perspective Projection
    vec3 rayDirection=normalize(virtualScreenCoord-eyePosition); 

	//
	// 1. find ray-cube intersect point 
	//

	int cnt=0;
	vec3 endPoint[2]; // 0 : entry point, 1 : exit point

	// .. ToDo

	float temp;

	float min_tx = (objectMin.x - eyePosition.x) / rayDirection.x;
	float max_tx = (objectMax.x - eyePosition.x) / rayDirection.x;
	if(max_tx < min_tx){
		temp = max_tx;
		max_tx = min_tx;
		min_tx = temp;
	}

	float min_ty = (objectMin.y - eyePosition.y) / rayDirection.y;
	float max_ty = (objectMax.y - eyePosition.y) / rayDirection.y;
	if(max_ty < min_ty){
		temp = max_ty;
		max_ty = min_ty;
		min_ty = temp;
	}

	float min_tz = (objectMin.z - eyePosition.z) / rayDirection.z;
	float max_tz = (objectMax.z - eyePosition.z) / rayDirection.z;
	if(max_tz < min_tz){
		temp = max_tz;
		max_tz = min_tz;
		min_tz = temp;
	}

	/* Largest min & Smallest max */
	float lmin = (min_tx > min_ty)? min_tx : min_ty;
	float smax = (max_tx < max_ty)? max_tx : max_ty;

	if(min_tx > max_ty || min_ty > max_tx)
		return;
	if(lmin > max_tz || min_tz > smax)
		return;

	if(min_tz > lmin) lmin = min_tz;
	if(max_tz < smax) smax = max_tz;

	// Entry point
	endPoint[0].x = eyePosition.x + lmin * rayDirection.x;
	endPoint[0].y = eyePosition.y + lmin * rayDirection.y;
	endPoint[0].z = eyePosition.z + lmin * rayDirection.z;

	// Exit point
	endPoint[1].x = eyePosition.x + smax * rayDirection.x;
	endPoint[1].y = eyePosition.y + smax * rayDirection.y;
	endPoint[1].z = eyePosition.z + smax * rayDirection.z;
	

#ifdef MIP
	//
	// 2. Maximum intensity projection
	//

    vec4 composedColor=vec4(0,0,0,0);

    // .. ToDo

	int samplingNum = int( length(endPoint[1] - endPoint[0]) * 100);
	vec3 normalizeWeight = 1.0 / (objectMax - objectMin);
	vec3 step = (endPoint[1] - endPoint[0]) * normalizeWeight / samplingNum;
	vec3 cur_loc = (endPoint[0] - objectMin) * normalizeWeight;

	float max_intensity = 0;

	for(int i=0; i<samplingNum; i++){
		/* from endPoint[0] to endPoint[1], go to step by step.
		   That is, updating current location. */
		cur_loc = cur_loc + step;

		float dataValue = texture3D(tex, cur_loc).x;

		/* just pick the maximum intensity among sampled values. */
		if(dataValue > max_intensity)
		  max_intensity = dataValue;
	}
	
	composedColor = vec4(max_intensity, max_intensity, max_intensity, max_intensity);

    gl_FragColor=composedColor;


#else
	//
	// 3. alpha composition
	//

	/* At the code < color = texture1D(transferFunction, dataValue); >

	   I think the cause of "Error validating program" in textfile.cpp is that
	   transferFunction is not initialized, so in fragment shader, it cannot take
	   the value from transferFunction. 
	   
	   So, I run this program after annotating validation part of textfile.cpp */

    float composedAlpha=0;
    vec4 composedColor=vec4(0,0,0,0);

    // .. ToDo

	int samplingNum = int( length(endPoint[1] - endPoint[0]) * 100);
	vec3 normalizeWeight = 1.0 / (objectMax - objectMin);
	vec3 step = (endPoint[1] - endPoint[0]) * normalizeWeight / samplingNum;
	vec3 cur_loc = (endPoint[0] - objectMin) * normalizeWeight;

	for(int i=0; i<samplingNum; i++){
		/* from endPoint[0] to endPoint[1], go to step by step.
		   That is, updating current location. */
		cur_loc = cur_loc + step;

		/* read the volume intensity at the given sampling location. */
		float dataValue = texture3D(tex, cur_loc).x;
		
		/* convert the intensity to RGBA value(color). */
		vec4 color = texture1D(transferFunction, dataValue);

		/* front-to-back compositing */
		composedColor = composedColor + (1-composedAlpha) * color.w * color;
		composedAlpha = composedAlpha + (1-composedAlpha) * color.w;

		/* early termination when alpha is close to 1.0 */
		if(composedAlpha > 0.9) break;
	}
	
    gl_FragColor=composedColor;
#endif
}