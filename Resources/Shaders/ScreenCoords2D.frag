#version 460 core

// Input variables from vertex shader
in vec4 color;
in vec2 texCoord;

uniform sampler2D textureSampler;

out vec4 fragColor;

void main()
{
    vec4 textureColor = texture(textureSampler, texCoord);

    fragColor = color * textureColor;
}