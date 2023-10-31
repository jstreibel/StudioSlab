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
uniform vec4 amb = vec4(.25f);

uniform int gridSubdivs; // Power of 2

uniform sampler2D field;

const int COLOR         = 0;
const int NORMALS       = 1;
const int GLOOM         = 2;

uniform int shading = COLOR;

uniform float scale;
uniform float gloomPowBase = 50;
uniform float gloomMultiplier = 1;

uniform vec2 texelSize;

// Input from vertex shader *************************
in vec3 v_normal;
in vec3 v_position;
in vec2 v_texcoord;

// Output
out vec4 fragColor;

void main()
{
    // Lighting ************************************
    vec3 normal = computeNormal(field, v_texcoord, texelSize, scale);
    vec4 l1 = vec4(light1_color * omni_brightness_no_decay(v_position, light1_position, normal), 1);
    vec4 l2 = vec4(light2_color * omni_brightness_no_decay(v_position, light2_position, normal), 1);
    vec4 l3 = vec4(light3_color * omni_brightness_no_decay(v_position, light3_position, normal), 1);

    vec4 lightingColor = amb + (1-amb)*(l1+l2+l3);

    // Grid ****************************************
    vec4 gridColor = vec4(vec3(0.2), 1);
    vec4 color = cartesianGrid(v_position.xy, gridColor, lightingColor, gridSubdivs);

    if     (shading == COLOR)       fragColor = color;
    else if(shading == NORMALS)     fragColor = vec4(.5*normal+.5, 1);
    else if(shading == GLOOM){
        vec3 cameraToPixel = normalize(eye-v_position);
        float gloom = gloomMultiplier*pow(gloomPowBase, 1-dot(cameraToPixel,normal))/gloomPowBase;
        fragColor = vec4(vec3(gloom), 1);
    }
}
