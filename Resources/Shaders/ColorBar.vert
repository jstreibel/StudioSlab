#version 460 core


layout (location = 0) in ivec2 inPosition;
layout (location = 2) in float inTexCoord;

uniform int vpWidth;
uniform int vpHeight;

out float texCoord;


void main()
{
    vec2 position = 2.0f * (vec2(inPosition) / vec2(vpWidth, vpHeight) - 0.5f);

    gl_Position = vec4(position, 0.0, 1.0);

    texCoord = inTexCoord;
}