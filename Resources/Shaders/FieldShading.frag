#version 460





/*
 * t <= 0    : return 0
 * 0 < t < 1 : return t
 * t >= 1    : return 0
 */
float
colormap_segment(float edge0, float edge1, float x)
{
    return step(edge0,x) * (1.0-step(edge1,x));
}

/*
 * t <= 0    : return under
 * 0 < t < 1 : return color
 * t >= 1    : return over
 */
vec3
colormap_underover(float t, vec3 color, vec3 under, vec3 over)
{
    return step(t,0.0)*under +
    colormap_segment(0.0,1.0,t)*color +
    step(1.0,t)*over;
}

/*
 * t <= 0    : return under
 * 0 < t < 1 : return color
 * t >= 1    : return over
 */
vec4
colormap_underover(float t, vec4 color, vec4 under, vec4 over)
{
    return step(t,0.0)*under +
    colormap_segment(0.0,1.0,t)*color +
    step(1.0,t)*over;
}





vec3 colormap_ice(float t)
{
    t = 1 - t;
    return mix(mix(vec3(1,1,1), vec3(0,1,1), t),
    mix(vec3(0,1,1), vec3(0,0,1), t*t), t);
}

vec3 colormap_ice(float t, vec3 under, vec3 over)
{
    return colormap_underover(t, colormap_ice(t), under, over);
}

vec4 colormap_ice(float t, vec4 under, vec4 over)
{
    return colormap_underover(t, vec4(colormap_ice(t),1.0), under, over);
}




vec3 colormap_fire(float t)
{
    return mix(mix(vec3(1,1,1), vec3(1,1,0), t),
    mix(vec3(1,1,0), vec3(1,0,0), t*t), t);
}

vec3 colormap_fire(float t, vec3 under, vec3 over)
{
    return colormap_underover(t, colormap_fire(t), under, over);
}

vec4 colormap_fire(float t, vec4 under, vec4 over)
{
    return colormap_underover(t, vec4(colormap_fire(t),1.0), under, over);
}




vec3 colormap_icefire(float t)
{
    return colormap_segment(0.0,0.5,t) * colormap_ice(2.0*(t-0.0)) +
    colormap_segment(0.5,1.0,t) * colormap_fire(2.0*(t-0.5));
}

vec3 colormap_icefire(float t, vec3 under, vec3 over)
{
    return colormap_underover(t, colormap_icefire(t), under, over);
}

vec4 colormap_icefire(float t, vec4 under, vec4 over)
{
    return colormap_underover(t, vec4(colormap_icefire(t),1.0), under, over);
}











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
uniform int shading = 0;
uniform int gridSubdivs;
uniform int showLevelLines;

uniform sampler2D field;

uniform float t;
uniform float phiScale;
uniform float dPhidtScale;

uniform vec2 dr_tex;
uniform vec2 dr;


uniform int viewMode;


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
    // Calculate normal in world coordinates
    vec3 n = normalize(normal * vec4(normal_vector,1.0)).xyz;

    // Calculate the location of this fragment (pixel) in world coordinates
    vec3 pos = vec3(view * model * vec4(v_position, 1));

    // Calculate the vector from this pixels surface to the light source
    vec3 surface_to_light = light_position - pos;

    // Calculate the cosine of the angle of incidence (brightness)
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
    vec4 fieldInfo = texture(field, v_texcoord);
    float Phi = phiScale * fieldInfo.r;
    float dPhidt = dPhidtScale*fieldInfo.g;


    vec4 color;
    //vec4 color = vec4(colormap_fire(dPhidt), 1);
    //vec4 color = vec4(colormap_blues(dPhidt), 1);
    if(viewMode == 0){
//!!!!! color = vec4(colormap_icefire(dPhidt+.5), 1);
        //color = vec4((v_texcoord-.5)*s+.5, 0, 1);
    }
    else if(viewMode == 1){
//!!!!! color = vec4(colormap_icefire(Phi+.5), 1);
    }
    else if(viewMode == 2){
//!!!!! color = vec4(colormap_icefire(floor(24*(Phi+.5))/24), 1);
    }

    //vec4 color = vec4(colormap_blues(Phi), 1);
    //vec4 color = vec4(1);


    // Veio normal mapping *************************
    {
        // color.rgb = (v_normal + 1)*.5;
    }


    // Level lines *********************************
    if(showLevelLines == 1)
    {
        vec3 red =       vec3(1,0,0);
        vec3 orange =    vec3(1, 0.647, 0);
        vec3 blackVoid = vec3(0);
        vec3 white =     vec3(1);

        vec3 lineColor = orange;
        float valueOffset = -1.e-3;

        //lineColor = lightOn==1 ? vec4(vec3(2),1) : vec4(0);

        //color = computeLine(Phi+valueOffset, vec4(2*(  lineColor), 1), color, 10, 2, 2);
        color = computeLine(Phi/phiScale+valueOffset, 1-color, color, 1, 3, 2);
        //color = computeLine(Phi/phiScale+valueOffset, 1-color, color, 2, 1, 1);
        ////color = computeLine(Phi+valueOffset, vec4(2*(1-lineColor), 1), color, 10, 1, 0.5);

        //valueOffset *= 10;
        //color = computeLine(dPhidt+valueOffset, vec4(lineColor, 1), color, 10, 2, 1);
        ////color = computeLine(dPhidt+1.5*valueOffset, vec4(red, 1), color, 10, 2, 1);
        //color = computeLine(dPhidt+valueOffset, 1-color, color, 10, 2, 1);
    }


    // Coordinates:
    float x = v_position.x, y = v_position.y;
    float r = length(v_position.xy), theta = atan(y/x)+.5*pi;



    // Light cone **********************************
    {
        float eps = 0.05;
        if(r > t-eps && r <= t)
            color = 0.5*color;
    }


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


    // Plane section *******************************
    //color = computeLine(y/2, vec4(1, 0.549, 0., 1), color, 1, 4, 3);
    //if(y>0) color.a = 0.0;


    // Lighting ************************************
    vec3 normal;// = v_normal;
    if(lightOn == 1)
    {
        vec2 dx=vec2(dr_tex.x, 0), dy=vec2(0, dr_tex.y);
        float fN, fS, fE, fW, scale;

        if(viewMode == 0){
            scale = phiScale;

            fN = texture2D(field, v_texcoord+dy).r;
            fS = texture2D(field, v_texcoord-dy).r;
            fE = texture2D(field, v_texcoord+dx).r;
            fW = texture2D(field, v_texcoord-dx).r;
        } else if(viewMode == 1) {
            scale = dPhidtScale;

            fN = texture2D(field, v_texcoord+dy).g;
            fS = texture2D(field, v_texcoord-dy).g;
            fE = texture2D(field, v_texcoord+dx).g;
            fW = texture2D(field, v_texcoord-dx).g;
        }

        float dfdx = scale*(fE-fW)/(2*dr.x);
        float dfdy = scale*(fN-fS)/(2*dr.y);

        vec3 normal = normalize(vec3(dfdx, dfdy, -1));
        vec4 l1 = vec4(light1_color * lighting(light1_position, normal), 1);
        vec4 l2 = vec4(light2_color * lighting(light2_position, normal), 1);
        vec4 l3 = vec4(light3_color * lighting(light3_position, normal), 1);

        float amb = 0.25;
        color *= (amb + (1-amb)*(l1+l2+l3));


        //color.rgb = -normal; //(normal+1)/2;
        //color.a=1;
    }

    if(shading==0)
        fragColor = color;
    else if(shading==1)
        fragColor = vec4(normal*.5+.5, 1);
    else if(shading==2)
        fragColor = vec4(v_texcoord, 1, 1);
}
