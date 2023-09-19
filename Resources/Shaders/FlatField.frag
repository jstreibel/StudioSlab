#version 460 compatibility

uniform sampler2D texture;

in vec2 TexCoord;
out vec4 FragColor;

void main()
{
    FragColor = texture2D(texture, TexCoord);
}
