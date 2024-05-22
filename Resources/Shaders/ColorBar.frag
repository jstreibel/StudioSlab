#version 460 core

// Input variables from vertex shader
in float texCoord;

uniform sampler1D colormap;

out vec4 fragColor;

/*
const float e1 = 1.7182818284590452354f;	// e-1

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
    float keps  = e1/(kappa);

    float Gamma = f(keps * phi_sat);
    float mu    = f(keps * abs(phi));

    float u = sign(phi)*mu/Gamma; // maps -phi_sat..phi_sat to -1..1 and 0 to 0

    if(symmetric) // map --1..1 to 0..1
    return .5f*(u+1);

    // map 0..1 to 0..1
    return u;
}
*/

void main()
{
    vec4 textureColor = texture(colormap, texCoord);

    fragColor = textureColor;
}