#version 460 compatibility

uniform sampler2D fieldData;
uniform sampler1D colormap;

uniform float phiMin;
uniform float phiMax;
uniform float eps;
uniform bool useLog = true;

in vec2 TexCoord;
out vec4 FragColor;

float logAbs(float phi){

    float s = sign(phi);
    float v = abs(phi);
    float cPhi = log(v/eps + 1)*s;

    return (cPhi-phiMin) / (phiMax-phiMin);
}

void main()
{
    float phi = texture2D(fieldData, TexCoord).r;

    vec4 color = texture(colormap, logAbs(phi));
    if(!useLog) color = texture(colormap, (phi-phiMin) / (phiMax-phiMin));

    FragColor = color;
}
