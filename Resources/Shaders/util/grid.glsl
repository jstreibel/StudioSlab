

const float gridAntialiasFactor = 1+1.e-2;

vec4 computeLine(float value,  vec4  line_color, vec4  bg_color,
                 float levels, float lineWidth,  float antialias)
{
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

vec4 polarGrid(vec2 xy, vec4 line_color, vec4 bg_color, int subdivs){
    const float pi = 3.14159265359;

    float r = length(xy), theta = atan(xy.y/xy.x)+.5*pi;

    float initialThickness = 3;
    vec4 color = bg_color;
    for (int l=1; l<=subdivs; l*=2){
        float antialias = pow(gridAntialiasFactor, l);
        color = computeLine(r,        line_color, color, l, initialThickness/l, antialias);
        color = computeLine(theta/pi, line_color, color, l, initialThickness/l, antialias);
    }

    return color;
}

vec4 cartesianGrid(vec2 xy, vec4 line_color, vec4 bg_color, int subdivs){
    float x = xy.x, y = xy.y;

    vec4 color = bg_color;
    for (int l=1; l<=subdivs; l*=2){
        float antialias = pow(gridAntialiasFactor, l);
        float thickness = 2./l;
        color = computeLine(x, line_color, color, l, 2./l, antialias);
        color = computeLine(y, line_color, color, l, 2./l, antialias);
    }

    return color;
}


