in vec2 v_texcoord;

uniform sampler2D field;

void main()
{
    vec2 color = texture(field, v_texcoord).rg;

    gl_FragColor = vec4(color, 0, 1);
}
