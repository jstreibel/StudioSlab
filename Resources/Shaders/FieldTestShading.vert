#version 460


mat4 basis = mat4(
    1.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 1.0
    );



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
    v_position = position;
    v_texcoord = texcoord;
    v_normal = normal;

    gl_Position = projection*modelview*basis*vec4(position, 1);
}