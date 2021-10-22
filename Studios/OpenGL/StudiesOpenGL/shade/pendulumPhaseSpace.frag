#version 330 core

#define PI 3.1415926538

out vec3 color;

//uniform int width;
//uniform int height;

in vec2 coord;

void main() {	
	float x = coord.x*5;
	float y = coord.y*5;
	
	float H = 0.5*y*y + (1-cos(x));
	
	float sH = sin(PI*H);
	
	
	float intensity = 1;
	const float threshold = 0.1;
	if(abs(sH) < threshold) intensity = 0;
	
	color = vec3(intensity);

}
