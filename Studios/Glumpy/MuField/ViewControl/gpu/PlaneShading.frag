#version 460

#include "colormaps/colormaps.glsl"

varying vec3 v_normal;
varying vec3 v_position;
varying vec2 v_texcoord;

uniform sampler2D field;
uniform vec2 tex_dr;
uniform vec2 SpaceSize;

uniform int colouring;
uniform int showLevelLines;

const float pi = 3.14159265359;

vec4 computeLevelLine(float value,
                 vec4 line_color, vec4 bg_color,
                 float levels=16,
                 float lineWidth=1, float antialias=1){
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

void main()
{
    float tex_dx = tex_dr.x, tex_dy = tex_dr.y;

    vec2 texCoord = v_texcoord;

    // Carregamento de dados, da memoria de textura pra ca
    vec2 center = texture2D(field, v_texcoord).rg;
    float thC = center.r;
    float thN = texture2D(field, v_texcoord+vec2(0, tex_dy)).r;
    float thS = texture2D(field, v_texcoord+vec2(0,-tex_dy)).r;
    float thE = texture2D(field, v_texcoord+vec2(tex_dx, 0)).r;
    float thW = texture2D(field, v_texcoord+vec2(-tex_dx, 0)).r;
    float dthdt = center.g;

    vec3 paintColor;
    if(colouring == 0){
        float e;
        {
            float stC = sin(thC), ctC = cos(thC),
            stN = sin(thN), ctN = cos(thN),
            stS = sin(thS), ctS = cos(thS),
            stE = sin(thE), ctE = cos(thE),
            stW = sin(thW), ctW = cos(thW);

            float k = dthdt*dthdt;

            float d = (SpaceSize*tex_dr).x;
            float u = -(1/d)*(ctC*(ctN+ctS+ctE+ctW) + stC*(stN+stS+stE+stW));

            e = k+u;
            e /= (4/d);// normaliza (o valor minimo de e eh o valor minimo global (na Hamiltoniana) de u ,
            // que eh min(u)=-4/d).
            e += 1;// deixa o minimo em zero (transalada 1 pra direita);
            e /= SpaceSize.x;// deixa na escala "correta de acordo com o tamanho do espaco";
            e *= 15;// reescala pra visualizacao;

            //const float eps=6.00e-2;
            //e = eps*(exp(abs(e)/eps + 1))*sign(e);
            //paintColor = colormap_wheel(e);
            //paintColor = colormap_wheel(1);
            //paintColor = colormap_hot(e);
            //paintColor = colormap_fire(e);
            //paintColor = colormap_icefire(e);
            //paintColor = colormap_gray(-0.05+1.1*e, vec3(0.5,0,0), vec3(0,0,0.5));
            paintColor = colormap_jet(e);
        }
    }
    else if(colouring == 1){
        paintColor = colormap_wheel(thC/(2*pi));
    }


    // "LINHAS DE NIVEL"
    vec4 color;
    if(showLevelLines != 0){
        vec4 line_color = vec4(0, 0, 0, 1);
        color = computeLevelLine(thC/(2*pi), line_color, vec4(paintColor,1), 8);
    } else color = vec4(paintColor, 1);


    // ZONAS DE DISSIPACAO
    float L = SpaceSize.x;
    float xmin = -L/2;
    float xmax = -xmin;
    float x = xmin + v_texcoord.x*L;
    float y = xmin + v_texcoord.y*L;
    float dbs = L*0.1;
    float dissRegion = xmin+dbs;
    //if(x<xmin+dbs || x>xmax-dbs || y<xmin+dbs || y>xmax-dbs) color *= 0.9;
    if(y<dissRegion){
        float s = -(y-dissRegion)/dbs;
        //color *= .5+.5*cos(pi*s);
        color *= 1-s;
    }
    else if(y>-dissRegion){
        float s = (y+dissRegion)/dbs;
        //color *= .5+.5*cos(pi*s);
        color *= 1-s;
    }

    if(x<dissRegion){
        float s = -(x-dissRegion)/dbs;
        //color *= .5+.5*cos(pi*s);
        color *= 1-s;
    }
    else if(x>-dissRegion){
        float s = (x+dissRegion)/dbs;
        //color *= .5+.5*cos(pi*s);
        color *= 1-s;
    }

    gl_FragColor = color;
}
