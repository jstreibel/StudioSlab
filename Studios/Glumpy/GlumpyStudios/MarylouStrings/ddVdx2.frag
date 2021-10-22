#version 460

//out vec4 fragd_color;
//layout(location=0) out vec4 color;

uniform float x;

varying vec2 v_spaceCoord;

const float pi = 3.14159265359;

void main() {
    vec2 r = v_spaceCoord;
    float h = r.x, dh = r.y;
    float g=0.1;
    float ddh=dh/x-x*x*(1-h)*g*g;

    gl_FragColor = vec4(ddh, 0, 0, 1); // dirac delta
}
