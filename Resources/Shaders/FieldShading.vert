#version 460

#include "util/heightmap_normal.glsl"

uniform mat4 modelview;
uniform mat4 projection;

uniform sampler2D field;
uniform float scale = 1.0;

// texture element size, in texture coords
uniform vec2 texelSize;

in vec2 position;
in vec2 texcoord;

out vec3 v_normal;
out vec3 v_position;
out vec2 v_texcoord;

void main()
{
    float height = scale*texture2D(field, texcoord).r;
    vec4 pos = vec4(vec3(position, height), 1);

    v_position = pos.xyz;
    v_texcoord = texcoord;
    v_normal = computeNormal(field, texcoord, texelSize, scale);

    gl_Position = projection * modelview * pos;
}