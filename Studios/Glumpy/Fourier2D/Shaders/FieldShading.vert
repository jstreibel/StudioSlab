#version 460

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

uniform vec2 dr;

uniform int viewMode;

uniform sampler2D field;

uniform float phiScale;
uniform float dPhidtScale;

uniform float t;
uniform float S;

attribute vec3 normal;
attribute vec3 position;
attribute vec2 texcoord;


varying vec3 v_normal;
varying vec3 v_position;
varying vec2 v_texcoord;

const float pi = 3.14159265359;

const float depth = 10;

void main()
{
    vec2 dx=vec2(dr.x, 0), dy=vec2(0, dr.y);
    vec3 newNormal;

    if(viewMode==0){
        float d = 1./32;
        float z = 0;
        vec2 r = position.xy;
        float dfdx=0, dfdy=0;
        for(float tx=d/2; tx<1; tx+=d){
            for(float ty=d/2; ty<1; ty+=d){
                vec2 FourierMapTexCoord = vec2(tx, ty); // vai de 0 ate um em 's' e em 't'
                float A = phiScale*texture2D(field, FourierMapTexCoord).r;

                vec2 k_v = S * (FourierMapTexCoord - 0.5)*2.0;
                float k = length(k_v);
                float em = exp(-k*depth),
                      ep = exp(+k*depth);
                float tanh = (ep-em) / (ep+em);
                float omega = sqrt(9.8*k*tanh);

                float kr = dot(k_v,r);
                float arg = kr + omega*t;
                z += A*cos(arg);

                float sinarg = sin(arg);
                dfdx -= A*k_v.x*sinarg;
                dfdy -= A*k_v.y*sinarg;
            }
        }

        newNormal = normalize(vec3(dfdx, dfdy, -1));

        v_position = position - z*normal;
    }

    v_texcoord = texcoord;
    v_normal = normalize(newNormal);
    gl_Position = <transform>;
}