#version 460 compatibility

uniform sampler2D texture;

in vec2 TexCoord;

void main()
{
    gl_FragColor = texture2D(texture, TexCoord);
}
