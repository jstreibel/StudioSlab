#version 460 core

// Input variables from vertex shader
in float texCoord;

uniform sampler1D colormap;

uniform float phi_max=1.0;
uniform float phi_min=-1.0;
uniform float phi_sat=1.0;
uniform float kappa=1.0;
uniform bool symmetric = true;
#define MODE_ALL_FIELD_VALUES     0
#define MODE_ONLY_VALUES_IN_SAT_RANGE 1
uniform int mode = MODE_ONLY_VALUES_IN_SAT_RANGE;


out vec4 fragColor;

const float em1 = 1.7182818284590452354f;	// e-1

float log1(float x) {
    if (abs(x) < 1e-4) {
        // Use a series expansion for better precision when x is small
        return x - 0.5 * x * x + x * x * x / 3.0;
    }

    return log(1.0 + x);
}

float f(float x) {
    // return tanh(x);
    return log1(x);
}

float map_to_tex_coord(float phi){
    float keps  = em1/(kappa);

    float Gamma = f(keps * phi_sat);
    float mu    = f(keps * abs(phi));

    float u = sign(phi)*mu/Gamma; // maps -phi_sat..phi_sat to -1..1 and 0 to 0

    if(symmetric) // map --1..1 to 0..1
    return .5f*(u+1);

    // map 0..1 to 0..1
    return u;
}

void main()
{
    float t = texCoord;
    float phi;
    if(mode == MODE_ALL_FIELD_VALUES)
        phi = t*(phi_max-phi_min)+phi_min;
    else if(mode == MODE_ONLY_VALUES_IN_SAT_RANGE) {
        if (symmetric)
            phi = phi_sat * (2 * t - 1);
        else
            phi = phi_sat * t;
    }

    vec4 textureColor = texture(colormap, map_to_tex_coord(phi));

    fragColor = textureColor;
}