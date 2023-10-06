#version 460

uniform mat4 modelview;
uniform mat4 projection;

uniform sampler2D field;

in vec3 normal;
in vec3 position;
in vec2 texcoord;
out vec3 v_normal;
out vec3 v_position;
out vec2 v_texcoord;

void main()
{
    float height = texture(field, texcoord).r;

    v_position = position;
    v_texcoord = texcoord;
    v_normal = normal;

    vec3 pos = position + height*normal;

    gl_Position = projection*modelview*vec4(pos, 1);
}