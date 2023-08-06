#version 460

//out vec4 fragd_color;
//layout(location=0) out vec4 color;

uniform float eps;
uniform float a;

in vec2 v_spaceCoord;

const float pi = 3.14159265359;

out uvec4 fragColor;

void main() {
    double r = length(v_spaceCoord);

    double val=0;

    //if(r<eps) val = a*(3/(pi*eps*eps)) * (1-r/eps);
    if(r<eps) val = (1-r/eps)/1.2e2;

    fragColor.rg = unpackDouble2x32(val);
    fragColor.ba = unpackDouble2x32(0.0); // dirac delta
}
