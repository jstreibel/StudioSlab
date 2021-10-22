#version 460

uniform float eps;
uniform float a;

varying vec2 v_spaceCoord;

void main() {
    vec2 r = v_spaceCoord.xy;

    float R=length(r);
    float th=atan(r.y/r.x);

    float arg = -R*R/(eps/eps);
    float phi = a*sin(arg);
    float dphidt = a*cos(arg);

     gl_FragColor = vec4(phi, dphidt, 0, 1); // dirac delta
    // frag_color = vec4(0, r, 0, 1);
}
