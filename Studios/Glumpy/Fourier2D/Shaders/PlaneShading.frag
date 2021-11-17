#include "misc/spatial-filters.frag"
#include "colormaps/colormaps.glsl"

varying vec3 v_normal;
varying vec3 v_position;
varying vec2 v_texcoord;

uniform sampler2D ft;

uniform float S;

const float pi = 3.14159265359;
const float gridAntialiasFactor = 1+1.e-2;

vec4 computeLine(float value,
                 vec4 line_color, vec4 bg_color,
                 float levels,
                 float lineWidth, float antialias){
    float width = lineWidth + antialias;

    ////if(length(value-0.5) < 0.5/levels)
    ////    width = lineWidth*2 + antialias;

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

vec4 polarGrid(vec2 r, vec4 line_color, vec4 bg_color){
    float R = length(r), theta = atan(r.y/r.x);

    vec4 color = bg_color;
    color = computeLine(R, line_color, color, 1, 2., 1);

    for (int l=1; l<=2; l*=2){
        float antialias = pow(gridAntialiasFactor, l);
        color = computeLine(theta/(pi), line_color, color, l, 2./l, antialias);
    }

    return color;
}

void main()
{
    vec2 k_v = S*(v_texcoord-0.5)*2;
    float k = length(k_v);
    float omega = 2*pi/length(k);
    vec2 T = normalize(k_v)*omega/(2*pi);

    vec2 T_texcoord = (.5*T/S)+0.5;
    //float A = texture2D(ft, v_texcoord).r;
    float A = texture2D(ft, T_texcoord).r;
    vec4 color = vec4(A);
    color.a=1;

    color = computeLine(2-A+1.e-3, vec4(0,1,0,1), color, 1, 1.8, 0.8);
    color = polarGrid(T, vec4(0.3,0.3, 0.3,1), color);

    if(k > S) color.a = 0;

    gl_FragColor = color;
}
