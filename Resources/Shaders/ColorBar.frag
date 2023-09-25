#version 460 core

// Input variables from vertex shader
in float texCoord;

uniform sampler1D colormap;

out vec4 fragColor;

void main()
{
    vec4 textureColor = texture(colormap, texCoord);

    fragColor = textureColor;
}