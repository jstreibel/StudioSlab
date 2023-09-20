#version 460 compatibility

uniform sampler2D fieldData;
uniform sampler1D colormap;

uniform float phiMin;
uniform float phiMax;
uniform float eps;
uniform bool useLog = true;

in vec2 TexCoord;
out vec4 FragColor;

float logAbs(float val){
    float s = sign(val);
    float v = abs(val);
    return log(v/eps + 1)*s;
}

void main()
{
    float phi = texture2D(fieldData, TexCoord).r;
    float cPhi = (phi-phiMin) / (phiMax-phiMin);

    vec4 color = texture(colormap, logAbs(cPhi));
    if(!useLog) color = texture(colormap, cPhi);

    FragColor = color;
}
