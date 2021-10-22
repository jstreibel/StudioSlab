#version 460

//out vec4 fragd_color;
//layout(location=0) out vec4 color;

uniform float eps;
uniform float a;

in vec2 v_spaceCoord;

const float pi = 3.14159265359;

//out vec4 fragColor;

void main() {
    float r = length(v_spaceCoord);
    float val=0;

    if(r<eps) val = a*(3/(pi*eps*eps)) * (1-r/eps);

    gl_FragColor.xy = vec2(0, val);

    //fragColor = vec4(0, val, 0, 1);
}
