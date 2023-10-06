#version 460

#include "util/heightmap_normal.glsl"
#include "util/lighting.glsl"
#include "util/grid.glsl"

// Input from host **********************************
uniform mat4 modelview;
uniform mat4 normal;
uniform mat4 projection;

uniform vec3 eye;

uniform vec3 light1_position;
uniform vec3 light2_position;
uniform vec3 light3_position;
uniform vec3 light1_color;
uniform vec3 light2_color;
uniform vec3 light3_color;
uniform vec3 ambientColor = vec3(.25f);

uniform int gridSubdivs; // Power of 2

uniform sampler2D field;

uniform float scale;

uniform vec2 texelSize;
uniform vec2 dr;

// Input from vertex shader *************************
in vec3 v_normal;
in vec3 v_position;
in vec2 v_texcoord;

// Output
out vec4 fragColor;

// Const ********************************************
const float pi = 3.14159265359;


void main()
{
    vec4 color = vec4(1);

    // Coordinates:
    float x = v_position.x,
          y = v_position.y;
    float r = length(v_position.xy),
          theta = atan(y/x)+.5*pi;

    // Grid ****************************************
    vec4 gridColor = vec4(vec3(0.2), 1);
    color = cartesianGrid(vec2(x, y), gridColor, color, gridSubdivs);

    // Lighting ************************************
    vec3 normal = computeNormal(field, v_texcoord, texelSize, scale);
    vec4 l1 = vec4(light1_color * lighting(v_position, light1_position, normal), 1);
    vec4 l2 = vec4(light2_color * lighting(v_position, light2_position, normal), 1);
    vec4 l3 = vec4(light3_color * lighting(v_position, light3_position, normal), 1);
    vec4 amb = vec4(ambientColor, 1.0);
    color *= (amb + (1-amb)*(l1+l2+l3));

    fragColor = color;
}
