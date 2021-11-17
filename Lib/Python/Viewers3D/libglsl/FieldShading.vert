#version 460

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

uniform vec2 dr_tex;

uniform int viewMode;

uniform sampler2D field;

uniform float phiScale;
uniform float dPhidtScale;

in vec3 normal;
in vec3 position;
in vec2 texcoord;
out vec3 v_normal;
out vec3 v_position;
out vec2 v_texcoord;

void main()
{
    v_position = (view*model*vec4(position,1)).xyz;

    v_texcoord = texcoord;
    v_normal = normal;
    gl_Position = <transform>;
}