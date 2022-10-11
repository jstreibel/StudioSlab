#version 460

uniform sampler2D field;
uniform float t;
uniform float dt;
uniform float drTex;
uniform float invhsqr;

in vec2 v_position;

out vec4 fragColor;

void main() {
    vec2 texcoord = (v_position+1)*.5;
    vec2 N = texcoord+vec2(0,      drTex),
         S = texcoord+vec2(0,     -drTex),
         E = texcoord+vec2( drTex, 0),
         W = texcoord+vec2(-drTex, 0),
         C = texcoord;

    vec2 phiN = texture(field, N).xy;
    vec2 phiS = texture(field, S).xy;
    vec2 phiE = texture(field, E).xy;
    vec2 phiW = texture(field, W).xy;
    vec2 phiC = texture(field, C).xy;
    float i_Phi =    phiC.r;
    float i_dPhidt = phiC.g;

    float o_Phi;
    float o_dPhidt;

    float r = length(v_position);

    if(r>t){
        o_Phi = 0.01;
        o_dPhidt = .0;
    } else {
        {
            o_Phi = i_Phi + i_dPhidt*dt;
        }

        {
            float ddx = phiW.r + phiE.r;
            float ddy = phiN.r + phiS.r;
            float laplacian = invhsqr*((ddx + ddy) - 4.*i_Phi);

            float dUdPhi = sign(i_Phi);
            //float dUdPhi = i_Phi;
            //float dUdPhi = sin(i_Phi);
            //float dUdPhi = 0;

            o_dPhidt = i_dPhidt + (laplacian - dUdPhi)*dt;
        }
    }


    fragColor.rg = vec2(o_Phi, o_dPhidt);
}
