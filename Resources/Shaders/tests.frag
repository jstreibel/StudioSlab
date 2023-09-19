#version 460 compatibility

uniform sampler2D texture;

in vec2 TexCoord;

void main()
{
    float val = texture2D(texture, TexCoord).r;
    gl_FragColor = vec4(val*val*val, val*val, val, 1);
}
