#version 330 

layout(location = 0) in vec3 spaceCoord;

uniform int width=800, height=600;
out vec3 coord;

void main(){
	coord = spaceCoord;
	
	gl_Position.x = spaceCoord.x*height/width;
	gl_Position.y = spaceCoord.y*height/width;
	gl_Position.z = 0.0;
	gl_Position.w = 1.0;	
}
