#version 460 compatibility

uniform sampler2D field_data; // The heightmap texture
uniform vec2 light_pos;       // The light source position in texture coordinates
uniform float scale;         // Scale factor for height values
uniform vec2 dr;        // 1/texture_resolution / size of one texel in texture coordinates

in vec2 TexCoord; // Texture coordinates
out vec4 FragColor; // Fragment color output

// Function to compute normal from heightmap
vec3 normal_from_heightmap(sampler2D height_data_on_red_channel, vec2 tex_coord, vec2 texel_size, float height_scale) {
    vec2 dr = texel_size;
    sampler2D height_data = height_data_on_red_channel;

    // Sample the height at the current and neighboring texels
    // float heightCenter = texture(height_data, tex_coord).r;

    float height_left   = texture(height_data, tex_coord - vec2(dr.x, 0.0)).r;
    float height_right  = texture(height_data, tex_coord + vec2(dr.x, 0.0)).r;
    float height_down   = texture(height_data, tex_coord - vec2(0.0, dr.y)).r;
    float height_up     = texture(height_data, tex_coord + vec2(0.0, dr.y)).r;

    // Calculate the gradient in the x and y directions
    float grad_x = height_scale*(height_right - height_left)/dr.x*.5f;
    float grad_y = height_scale*(height_up    - height_down)/dr.y*.5f;

    // Create the normal vector using the gradients
    vec3 normal = vec3(-grad_x, -grad_y, 1.0);

    // Normalize the vector to get the final normal
    return normalize(normal);
}

void main() {
    // Compute the surface normal using the gradients
    vec3 normal = normal_from_heightmap(field_data, TexCoord, dr, scale);

    // Compute the direction to the light source
    vec2 lightDir2D = normalize(light_pos - TexCoord );
    vec3 lightDir = normalize(vec3(lightDir2D, 1.0));

    // Compute the dot product between the normal and the light direction
    float lightIntensity = dot(normal, lightDir);

    // Clamp the light intensity to the range [0, 1]
    lightIntensity = clamp(lightIntensity, 0.0, 1.0);

    // Output the color based on the light intensity
    FragColor = vec4(vec3(lightIntensity), 1.0);
}