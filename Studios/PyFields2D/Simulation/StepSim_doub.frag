#version 460

uniform sampler2D phi;
uniform vec2 scale;
uniform float t;
uniform float dt;
uniform vec2 GridSize;
uniform float invhsqr;
uniform vec2 InverseGridSize;

in vec2 v_position;

out uvec4 fragColor;

dvec2 texelFetchOffset(usampler2D sampler, ivec2 P, ivec2 offset) {
    uvec4 texel = texture(sampler, vec2(P+offset)*InverseGridSize);
    return dvec2(packDouble2x32(texel.rg),
                 packDouble2x32(texel.ba));
}
dvec2 texelFetch      (usampler2D sampler, ivec2 P) {
    uvec4 texel = texture(sampler, vec2(P)*InverseGridSize);
    return dvec2(packDouble2x32(texel.rg),
                 packDouble2x32(texel.ba));
}


void main() {
    vec2 texelSize = InverseGridSize;

    // vec4 val = texture2D(phi, texCoord);

    ivec2 T = ivec2((v_position + 1)*.5 * GridSize);

    dvec2 phiN = texelFetchOffset(phi, T, ivec2(0, 1));
    dvec2 phiS = texelFetchOffset(phi, T, ivec2(0, -1));
    dvec2 phiE = texelFetchOffset(phi, T, ivec2(1, 0));
    dvec2 phiW = texelFetchOffset(phi, T, ivec2(-1, 0));
    dvec2 phiC = texelFetch(phi, T);
    double i_Phi = phiC.r;
    double i_dPhidt = phiC.g;
    double o_Phi;
    double o_dPhidt;

    double _dt = dt;

    {
        o_Phi = i_Phi + i_dPhidt*_dt;
    }

    {

        double ddx = phiW.r + phiE.r;
        double ddy = phiN.r + phiS.r;
        double laplacian = invhsqr*((ddx + ddy) - 4.*i_Phi);
        double dUdPhi = sign(i_Phi);
        //float dUdPhi = i_Phi;
        //float dUdPhi = sin(i_Phi);
        //float dUdPhi = 0;

        o_dPhidt = i_dPhidt + (laplacian + dUdPhi)*_dt;
    }

    // gl_FragColor.rg = vec2(o_Phi, o_dPhidt);
    fragColor.rg = unpackDouble2x32(o_Phi);
    fragColor.ba = unpackDouble2x32(o_dPhidt);
}
