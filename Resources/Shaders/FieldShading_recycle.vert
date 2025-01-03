#version 460





uniform mat4 modelview;
uniform mat4 projection;

// texture element size. In texture coords?
uniform vec2 dr_tex;

uniform int viewMode;

uniform sampler2D field;

uniform float phiScale;
uniform float dPhidtScale;

in vec3 normal;
in vec3 position;
in vec2 texcoord;
out vec3 v_normal;
out vec3 v_position;
out vec2 v_texcoord;

void main()
{
    vec2 dx=vec2(dr_tex.x, 0), dy=vec2(0, dr_tex.y);
    vec3 newNormal;

    if(viewMode==0){
           //float fN = phiScale * texture2D(field, texcoord+dy).r;
           //float fS = phiScale * texture2D(field, texcoord-dy).r;
           //float fE = phiScale * texture2D(field, texcoord+dx).r;
           //float fW = phiScale * texture2D(field, texcoord-dx).r;
           //float fC = phiScale * texture2D(field, texcoord+dr_tex*0.5).r;

        vec4 phi = texture(field, texcoord);
        float fC = phiScale * phi.r;

           //float dfdx = dFdx(fC);
           //float dfdy = dFdy(fC);

           //float dfdx = (fW-fE) / dr.x;
           //float dfdy = (fN-fS) / dr.y;
           //newNormal = normalize(vec3(dfdx, dfdy, -1));

        v_position = position + fC*normal;
    }
    else if(viewMode==1){
        //float fN = dPhidtScale * texture2D(field, texcoord+dy).g;
        //float fS = dPhidtScale * texture2D(field, texcoord-dy).g;
        //float fE = dPhidtScale * texture2D(field, texcoord+dx).g;
        //float fW = dPhidtScale * texture2D(field, texcoord-dx).g;
        //float fC = dPhidtScale * texture2D(field, texcoord).g;

        //float dfdx = -(fE-fW) / dr_tex.x;
        //float dfdy = -(fN-fS) / dr_tex.y;
        //newNormal = normalize(vec3(dfdx, dfdy, -1));
        vec4 phi = texture(field, texcoord);
        float fC = phiScale * phi.g;

        v_position = position + fC*normal;
    }
    else if(viewMode==2){
        float fC = phiScale * texture2D(field, texcoord).r;
        newNormal = vec3(0);

        v_position = position - floor(24*fC + 0.1)*normal/24;
    }

    v_texcoord = texcoord;
    v_normal = normal;

    gl_Position = projection*modelview*vec4(position, 1);
}