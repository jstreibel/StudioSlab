#version 420

attribute vec3 normal;
attribute vec3 position;
attribute vec2 texcoord;
varying vec3 v_normal;
varying vec3 v_position;
varying vec2 v_texcoord;

uniform sampler2D texture;

void main()
{
    v_texcoord = texcoord;
    v_normal = normal;
    v_position = position;

    gl_Position = <transform>;
}