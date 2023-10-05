#version 460

#include "colormaps/IceFire.glsl"


// Input from host **********************************
uniform mat4 modelview;
uniform mat4 normal;
uniform mat4 projection;

uniform vec3 light1_position;
uniform vec3 light2_position;
uniform vec3 light3_position;
uniform vec3 light1_color;
uniform vec3 light2_color;
uniform vec3 light3_color;

uniform int gridSubdivs; // Power of 2

uniform sampler2D field;

uniform vec2 dr_tex;
uniform vec2 dr;

// Input from vertex shader *************************
in vec3 v_normal;
in vec3 v_position;
in vec2 v_texcoord;


// Output
out vec4 fragColor;


// Const ********************************************
const float pi = 3.14159265359;
const float gridAntialiasFactor = 1+1.e-2;

float lighting(vec3 light_position, vec3 normal_vector)
{
    vec3 n = normalize(normal * vec4(normal_vector,1.0)).xyz;
    vec3 pos = vec3(modelview * vec4(v_position, 1));

    vec3 surface_to_light = light_position - pos;

    float d = length(surface_to_light);
    float brightness = dot(n, surface_to_light) /
    (d * length(n));
    brightness = max(min(brightness,1.0),0.0);
    return brightness;
}


vec4 computeLine(float value,
vec4 line_color, vec4 bg_color,
float levels, float lineWidth, float antialias){
    float width = lineWidth + antialias;

    float v  = levels*value - 0.5;
    float dv = width/2.0 * fwidth(v);
    float f = abs(fract(v) - 0.5);
    float d = smoothstep(-dv,+dv,f);

    float t = width/2.0 - antialias;
    d = abs(d)*width/2.0 - t;
    if( d < 0.0 ) return bg_color;

    d /= antialias;
    return mix(line_color, bg_color, d);
}


vec4 cartesianGrid(vec2 xy, vec4 line_color, vec4 bg_color){
    float x = xy.x, y = xy.y;

    vec4 color = bg_color;
    for (int l=1; l<=gridSubdivs; l*=2){
        float antialias = pow(gridAntialiasFactor, l);
        float thickness = 2./l;
        color = computeLine(x, line_color, color, l, 2./l, antialias);
        color = computeLine(y, line_color, color, l, 2./l, antialias);
    }

    return color;
}

void main()
{
    vec4 fieldInfo = texture(field, v_texcoord);
    float Phi = fieldInfo.r*2.f - 1.f;

    // vec4 color;
    // color = vec4(colormap_icefire(dPhidt+.5), 1);
    // color = vec4(colormap_icefire(Phi), 1);
    //vec4 color = vec4(colormap_blues(Phi), 1);
    vec4 color = vec4(1);

    // Coordinates:
    float x = v_position.x,
          y = v_position.y;
    float r = length(v_position.xy),
          theta = atan(y/x)+.5*pi;

    // Grid ****************************************
    {
        vec4 gridColor = vec4(vec3(0.2), 1);
        //vec4 gridColor = vec4(1-color.rgb, 1);
        color = cartesianGrid(vec2(x, y), gridColor, color);
    }

    // Lighting ************************************
    vec3 normal;// = v_normal;

    if(false)
    {
        vec2 dx=vec2(dr_tex.x, 0),
             dy=vec2(0, dr_tex.y);
        float fN, fS, fE, fW,
                scale;

        scale = 1.f;

        fN = texture2D(field, v_texcoord+dy).r;
        fS = texture2D(field, v_texcoord-dy).r;
        fE = texture2D(field, v_texcoord+dx).r;
        fW = texture2D(field, v_texcoord-dx).r;

        float dfdx = scale*(fE-fW)/(2*dr.x);
        float dfdy = scale*(fN-fS)/(2*dr.y);

        vec3 normal = normalize(vec3(dfdx, dfdy, -1));
        vec4 l1 = vec4(light1_color * lighting(light1_position, normal), 1);
        vec4 l2 = vec4(light2_color * lighting(light2_position, normal), 1);
        vec4 l3 = vec4(light3_color * lighting(light3_position, normal), 1);

        float amb = 0.25;
        color *= (amb + (1-amb)*(l1+l2+l3));
    }

    fragColor = color;
}
