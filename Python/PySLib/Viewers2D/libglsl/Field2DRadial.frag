#version 460

#include "misc/spatial-filters.frag"
#include "colormaps/colormaps.glsl"


// Input from host **********************************
uniform mat4 view;
uniform mat4 model;
uniform mat4 normal;
uniform mat4 projection;

uniform vec3 light1_position;
uniform vec3 light2_position;
uniform vec3 light3_position;
uniform vec3 light1_color;
uniform vec3 light2_color;
uniform vec3 light3_color;

#define CARTESIAN 1
#define POLAR 2
uniform int gridType;
uniform int lightOn;
uniform int gridSubdivs;
uniform int showLevelLines;

uniform sampler1D field;

uniform float t;
uniform float phiScale;
uniform float dPhidtScale;

uniform float dr_tex;
uniform float dr;


// Input from vertex shader *************************
varying vec3 v_normal;
varying vec3 v_position;
varying float v_rTex;


// Const ********************************************
const float pi = 3.14159265359;
const float gridAntialiasFactor = 1+1.e-2;

out vec4 fragColor;

float lighting(vec3 light_position, vec3 normal_vector)
{
    // Calculate normal in world coordinates
    vec3 n = normalize(normal * vec4(normal_vector,1.0)).xyz;

    // Calculate the location of this fragment (pixel) in world coordinates
    vec3 position = vec3(view * model * vec4(v_position, 1));

    // Calculate the vector from this pixels surface to the light source
    vec3 surface_to_light = light_position - position;

    // Calculate the cosine of the angle of incidence (brightness)
    float d = length(surface_to_light);
    float brightness = dot(n, surface_to_light) /
    (d * length(n));
    brightness = max(min(brightness,1.0),0.0);
    return brightness;
}


vec4 computeLine(float value,
                 vec4 line_color, vec4 bg_color,
                 float levels,
                 float lineWidth, float antialias){
    float width = lineWidth + antialias;

    //if(length(value-0.5) < 0.5/levels)
    //    width = lineWidth*2 + antialias;

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
        {
            //x*=10; y*=10;
            //color = computeLine(.5*y*y + 1-cos(x), line_color, color, l, 2./l, antialias);
        }
    }

    return color;
}
vec4 polarGrid(vec2 rv, vec4 line_color, vec4 bg_color){
    float r = rv.x, theta = rv.y;

    float initialThickness = 3;
    vec4 color = bg_color;
    for (int l=1; l<=gridSubdivs; l*=2){
        float antialias = pow(gridAntialiasFactor, l);
        color = computeLine(r,        line_color, color, l, initialThickness/l, antialias);
        color = computeLine(theta/pi, line_color, color, l, initialThickness/l, antialias);
    }

    return color;
}


void main()
{
    float rTex=v_rTex;
    vec4 fieldInfo = texture1D(field, rTex);
    float Phi = phiScale * fieldInfo.r;
    float dPhidt = dPhidtScale*fieldInfo.g;


    vec4 color = vec4(colormap_icefire(dPhidt+.5), 1);


    // Level lines *********************************
    if(showLevelLines == 1)
    {
        vec3 red =       vec3(1,0,0);
        vec3 orange =    vec3(1, 0.647, 0);
        vec3 blackVoid = vec3(0);
        vec3 white =     vec3(1);

        vec3 lineColor = orange;
        float valueOffset = -1.e-3;

        color = computeLine(Phi/phiScale+valueOffset, 1-color, color, 1, 3, 2);
    }


    // Space coordinates:
    float x = v_position.x, y = v_position.y;
    float r = length(v_position.xy), theta = atan(y/x)+.5*pi;


    // Grid ****************************************
    {
        vec4 gridColor = vec4(vec3(0.2), 1);
        //vec4 gridColor = vec4(1-color.rgb, 1);
        if (gridType == CARTESIAN){
            color = cartesianGrid(vec2(x, y), gridColor, color);
        }
        else if (gridType == POLAR){
            color = polarGrid(vec2(r, theta), gridColor, color);
        }
    }


    // Lighting ************************************
    //if(lightOn == 1)
    //{
    //    vec2 dx=vec2(dr_tex.x, 0), dy=vec2(0, dr_tex.y);
    //    float fN, fS, fE, fW, scale;
//
    //    float scale = phiScale;
//
    //    float fN = texture2D(field, v_texcoord+dy).r;
    //    float fS = texture2D(field, v_texcoord-dy).r;
    //    float fE = texture2D(field, v_texcoord+dx).r;
    //    float fW = texture2D(field, v_texcoord-dx).r;
//
    //    float dfdx = scale*(fE-fW)/(2*dr.x);
    //    float dfdy = scale*(fN-fS)/(2*dr.y);
//
    //    vec3 normal = normalize(vec3(dfdx, dfdy, -1));
    //    vec4 l1 = vec4(light1_color * lighting(light1_position, normal), 1);
    //    vec4 l2 = vec4(light2_color * lighting(light2_position, normal), 1);
    //    vec4 l3 = vec4(light3_color * lighting(light3_position, normal), 1);
//
    //    float amb = 0.25;
    //    color *= (amb + (1-amb)*(l1+l2+l3));
//
    //    // Debug:
    //    //color.rgb = -normal; //(normal+1)/2;
    //    //color.a=1;
    //}


    fragColor = color;
}
