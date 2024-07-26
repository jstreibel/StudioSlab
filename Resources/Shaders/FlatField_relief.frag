#version 460 compatibility

uniform sampler2D field_data;
uniform float light_zenith;
uniform float light_azimuth;
uniform float scale;
uniform vec2 dr;        // 1/texture_resolution / size of one texel in texture coordinates

in vec2 TexCoord;
out vec4 FragColor;

vec3 normal_from_heightmap(sampler2D height_data, vec2 tex_coord, vec2 texel_size, float height_scale) {
    vec2 dr = texel_size;

    float height_center  = texture(height_data, tex_coord).r;

    float height_left   = texture(height_data, tex_coord - vec2(dr.x, 0.0)).r;
    float height_right  = texture(height_data, tex_coord + vec2(dr.x, 0.0)).r;
    float height_down   = texture(height_data, tex_coord - vec2(0.0, dr.y)).r;
    float height_up     = texture(height_data, tex_coord + vec2(0.0, dr.y)).r;

    // float grad_x = height_scale*(height_right - height_left); ///dr.x*.5f;
    // float grad_y = height_scale*(height_up    - height_down); ///dr.y*.5f;
    float grad_x = height_scale*(height_right - height_center);
    float grad_y = height_scale*(height_up    - height_center);

    vec3 normal = vec3(-grad_x, -grad_y, 1.0);

    return normalize(normal);
}

void main() {
    vec3 normal = normal_from_heightmap(field_data, TexCoord, dr, scale);

    const float cosz = cos(light_zenith);
    const float sinz = sin(light_zenith);
    vec3 light_dir = vec3(cosz * cos(light_azimuth),
                          cosz * sin(light_azimuth),
                          sinz);
    float lightIntensity = dot(normal, light_dir);

    lightIntensity = clamp(lightIntensity, 0.0, 1.0);

    FragColor = vec4(vec3(lightIntensity), 1.0);
    // FragColor = vec4(vec3(normal), 1.0);
}