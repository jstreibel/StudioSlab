#version 460 compatibility

uniform sampler2D fieldData;
uniform sampler1D colormap;

in vec2 TexCoord;
out vec4 FragColor;

void main()
{
    float phi = texture2D(fieldData, TexCoord).r;

    FragColor = texture(colormap, 0.5f+phi);
}
