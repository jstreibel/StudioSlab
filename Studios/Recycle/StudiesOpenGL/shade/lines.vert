#version 330 

#define PI 3.14159

layout(location = 0) in float xPos;

uniform int width=800, height=600;
uniform float yPos, xRot, yRot, phase;
uniform bool vert = false;

void main(){	
	float ratio = width/height;
	
	float x, y, z;
	
	if(vert){
		x = yPos;
		y = xPos*ratio;
	} else {
		x = xPos*ratio;
		y = yPos;
	}
	
	vec2 k1 = 0.5*PI*vec2(3.8*PI, PI/4);
	float a1 = 0.04;
	vec2 k2 = 0.5*PI*vec2(2.4*PI, PI/4.1);
	float a2 = 0.03;
	
	vec2 r = vec2(x, y);
	z = a1*sin(dot(k1, r) + phase) + a2*sin(dot(k2, r) + phase);
	
	float alpha = xRot, beta = yRot;
	float ca=cos(alpha), cb=cos(beta), sa=sin(alpha), sb=sin(beta);
	
	float s = 0.8;
	gl_Position.xyz = s*x*vec3(cb, 0, - sb) 
					+ s*y*vec3(sb*sa, ca, cb*sa)
					+ s*z*vec3(sb*ca, -sa, cb*ca);
	
	gl_Position.w = 1.0;	
}
