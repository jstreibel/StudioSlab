#version 460

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

uniform float dr_tex;

uniform sampler1D field;

uniform float phiScale;
uniform float dPhidtScale;

attribute vec3 normal;
attribute vec3 position;
attribute vec2 texcoord;
varying vec3 v_normal;
varying vec3 v_position;
varying float v_rTex;


void main()
{
    float rTex = length(texcoord-.5) + .5;

    float f = phiScale * texture1D(field, rTex).r;

    v_position = position + f*normal;

    v_rTex = rTex;
    //v_normal = normalize(normal + newNormal);
    gl_Position = <transform>;
}