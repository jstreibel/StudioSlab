#version 460

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

uniform vec2 dr_tex;

uniform int viewMode;

uniform sampler2D field;

uniform float phiScale;
uniform float dPhidtScale;

attribute vec3 normal;
attribute vec3 position;
attribute vec2 texcoord;
varying vec3 v_normal;
varying vec3 v_position;
varying vec2 v_texcoord;


void main()
{
    vec2 dx=vec2(dr_tex.x, 0), dy=vec2(0, dr_tex.y);
    vec3 newNormal;

    if(viewMode==0){
        //float fN = phiScale * texture2D(field, texcoord+dy).r;
        //float fS = phiScale * texture2D(field, texcoord-dy).r;
        //float fE = phiScale * texture2D(field, texcoord+dx).r;
        //float fW = phiScale * texture2D(field, texcoord-dx).r;
        float fC = phiScale * texture2D(field, texcoord).r;

        //float dfdx = dFdx(fC);
        //float dfdy = dFdy(fC);

        //float dfdx = (fW-fE) / dr.x;
        //float dfdy = (fN-fS) / dr.y;
        //newNormal = normalize(vec3(dfdx, dfdy, -1));

        //v_position = position - abs(fC)*normal;
        v_position = position - fC*normal;
    }
    else if(viewMode==1){
        float fN = dPhidtScale * texture2D(field, texcoord+dy).g;
        float fS = dPhidtScale * texture2D(field, texcoord-dy).g;
        float fE = dPhidtScale * texture2D(field, texcoord+dx).g;
        float fW = dPhidtScale * texture2D(field, texcoord-dx).g;
        float fC = dPhidtScale * texture2D(field, texcoord).g;

        float dfdx = -(fE-fW) / dr_tex.x;
        float dfdy = -(fN-fS) / dr_tex.y;
        newNormal = normalize(vec3(dfdx, dfdy, -1));

        v_position = position - fC*normal;
    }
    else if(viewMode==2){
        float fC = phiScale * texture2D(field, texcoord).r;
        newNormal = vec3(0);

        v_position = position - floor(24*fC + 0.1)*normal/24;
    }

    v_texcoord = texcoord;
    //v_normal = normalize(normal + newNormal);
    gl_Position = <transform>;
}