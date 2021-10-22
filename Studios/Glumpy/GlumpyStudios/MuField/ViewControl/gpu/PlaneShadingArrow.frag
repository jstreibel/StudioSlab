#include "math/constants.glsl"
#include "arrows/arrows.glsl"
#include "antialias/antialias.glsl"

#include "colormaps/colormaps.glsl"

const float pi = 3.14159265358979323846;

varying vec3 v_normal;
varying vec3 v_position;
varying vec2 v_texcoord;

uniform sampler2D field;

uniform vec2 iResolution;
uniform float th0=0;
void main()
{
    const float SQRT_2 = 1.4142135623730951;
    const float linewidth = 1;
    const float antialias =  1;

    const float rows = 32.0;
    const float cols = 32.0;
    float body = min(iResolution.x/cols, iResolution.y/rows) / SQRT_2;
    vec2 texcoord = gl_FragCoord.xy;
    vec2 size   = iResolution.xy / vec2(cols,rows);
    vec2 center = (floor(texcoord/size) + vec2(0.5,0.5)) * size;



    texcoord -= center;

    //float theta = M_PI/3.0 + 0.1*(center.x / cols + center.y / rows);
    float theta= texture2D(field, v_texcoord*1).r + th0;
    //float theta = M_PI-atan(center.y-iMouse.y,  center.x-iMouse.x);

    float cos_theta = cos(theta);
    float sin_theta = sin(theta);
    texcoord = vec2(cos_theta*texcoord.x - sin_theta*texcoord.y,
                    sin_theta*texcoord.x + cos_theta*texcoord.y);

    //float d = arrow_curved(texcoord, body, 0.25*body, linewidth, antialias);
    //float d = arrow_stealth(texcoord, body, 0.25*body, linewidth, antialias);
    //float d = arrow_triangle_90(texcoord, body, 0.15*body, linewidth, antialias);
    //float d = arrow_triangle_60(texcoord, body, 0.20*body, linewidth, antialias);
    float d = arrow_triangle_30(texcoord, body, 0.25*body, linewidth, antialias);
    //float d = arrow_angle_90(texcoord, body, 0.15*body, linewidth, antialias);
    //float d = arrow_angle_60(texcoord, body, 0.20*body, linewidth, antialias);
    //float d = arrow_angle_30(texcoord, body, 0.25*body, linewidth, antialias);

    vec4 color = vec4(colormap_wheel(theta/(2*pi)), 1);
    //gl_FragColor = color;
    gl_FragColor = stroke(d, linewidth, antialias, vec4(0,0,0,1));
}