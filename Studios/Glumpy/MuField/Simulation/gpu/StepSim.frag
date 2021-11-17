#version 460

uniform sampler2D thetaField;
uniform vec2 scale;
uniform float t;
uniform float dt;
uniform vec2 GridSize;
uniform vec2 SpaceSize;
uniform vec2 InverseGridSize;

const float pi = 3.14159265359;

// FUNDAMENTAL: como esse eh um shader pra COMPUTACAO, nao precisamos de coords. de textura e usamos, instead,
// a propria posicao dos vertices como coordenadas de textura.
varying vec2 v_position;

vec4 texelFetchOffset(sampler2D sampler, ivec2 P, ivec2 offset)
{    return texture2D(sampler, vec2(P+offset)*InverseGridSize);}
vec4 texelFetch      (sampler2D sampler, ivec2 P)
{    return texture2D(sampler, vec2(P       )*InverseGridSize);}


void main() {
    vec2 texelSize = InverseGridSize;

    // vec4 val = texture2D(phi, texCoord);

    ivec2 T = ivec2((v_position + 1)*.5 * GridSize);

    vec2 thetaN = texelFetchOffset(thetaField, T, ivec2(0, 1)).rg;
    vec2 thetaS = texelFetchOffset(thetaField, T, ivec2(0, -1)).rg;
    vec2 thetaE = texelFetchOffset(thetaField, T, ivec2(1, 0)).rg;
    vec2 thetaW = texelFetchOffset(thetaField, T, ivec2(-1, 0)).rg;
    vec2 thetaC = texelFetch(thetaField, T).rg;

    float i_theta = thetaC.r;
    float i_dthetadt = thetaC.g;
    float o_theta;
    float o_dthetadt;


    {
        o_theta = i_theta + i_dthetadt*dt;
    }

    {
        float stC = sin(thetaC.r), ctC = cos(thetaC.r),
              stE = sin(thetaE.r), ctE = cos(thetaE.r),
              stS = sin(thetaS.r), ctS = cos(thetaS.r),
              stW = sin(thetaW.r), ctW = cos(thetaW.r),
              stN = sin(thetaN.r), ctN = cos(thetaN.r);

        float d = (SpaceSize*InverseGridSize).x;
        float dUdtheta = (1./d)*(-stC*(ctE+ctS+ctW+ctN) + ctC*(stE+stS+stW+stN));

        float a=0;
        vec2 r = .5*v_position*SpaceSize;
        float L = SpaceSize.x;
        float dissipationFactor=1e-3;
        float dissipationBorderSize = L*0.1;
        float dbs = dissipationBorderSize;
        float xmin = -L/2;
        float xmax = -xmin;
        float dissRegion = xmin+dbs;
        float x = r.x;
        float y = r.y;

        if(y<dissRegion) a += dissipationFactor*(y-dissRegion)/dbs; // || x>xmax-dbs || y<xmin+dbs || y>xmax-dbs) a=1.e-10;
        else if(y>-dissRegion) a += -dissipationFactor*(y+dissRegion)/dbs;

        if(x<dissRegion) a += dissipationFactor*(x-dissRegion)/dbs;
        else if(x>-dissRegion) a += -dissipationFactor*(x+dissRegion)/dbs;

        o_dthetadt = (1+a)*i_dthetadt + dUdtheta*dt;
    }

    gl_FragColor.rg = vec2(o_theta, o_dthetadt);
}
