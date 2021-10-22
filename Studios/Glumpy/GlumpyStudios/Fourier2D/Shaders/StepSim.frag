#version 460

uniform float t;
uniform float k_;
uniform vec2 spaceDim;
uniform float k0, th0, eps;
uniform float S;


varying vec2 v_position;

const float pi = 3.14159265359;

void main() {
    mat2 Rot;
    Rot[0].xy = vec2(cos(th0), -sin(th0)); // col 1
    Rot[1].xy = vec2(sin(th0),  cos(th0)); // col 2

    vec2 r = v_position.xy*S*Rot; // position eh de -1 ate 1 em 'x' e em 'y'.

    r -= k0;
    float x = r.x;
    float y = r.y;

    float R = length(r);
    float th = atan(r.y/r.x);

    float o_Phi = (1/(k0*sqrt(eps)))*exp(-(R*R)/(k0*k0*eps));
    float o_dPhidt = 0;

    gl_FragColor.rg = vec2(o_Phi, o_dPhidt);
}
