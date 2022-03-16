#version 420

attribute vec3 normal;
attribute vec3 position;
attribute vec2 texcoord;
varying vec3 v_normal;
varying vec3 v_position;
varying vec2 v_texcoord;

uniform mat2 Rot=mat2(1,0,
                      0,1);
uniform vec2 Trans=vec2(0,0);
uniform vec2 Scale; //=vec2(1,1);

uniform sampler2D field;

void main()
{
    vec2 pos = position.xy;
    pos.x *= Scale.x; pos.y *= Scale.y;
    pos = (pos-Trans)*Rot;

    v_texcoord = texcoord;
    v_normal = normal;
    v_position = vec3(pos.xy, position.z);

    gl_Position = vec4(pos.xy, position.z, 1);
}