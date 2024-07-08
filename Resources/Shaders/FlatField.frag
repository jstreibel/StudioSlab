#version 460 compatibility

uniform sampler2D field_data;
uniform sampler1D colormap;

uniform float phi_sat;
uniform float kappa;
uniform bool symmetric = true;
uniform float eps = .0f;

in vec2 TexCoord;
out vec4 FragColor;

const float e1 = 1.7182818284590452354f;	/* e-1 */

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

void main()
{
    float phi = eps + texture2D(field_data, TexCoord).r;

    vec4 color = texture(colormap, map_to_tex_coord(phi));
    FragColor = color;
}
