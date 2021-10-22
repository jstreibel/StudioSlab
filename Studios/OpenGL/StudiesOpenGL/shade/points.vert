#version 330 

#define PI 3.14159

layout(location = 0) in vec2 r;

void main(){		
	gl_Position = vec4(r.x, r.y, 0, 1);
}
