#version 460 compatibility

uniform sampler2D field_data; // The heightmap texture
uniform vec2 light_pos;       // The light source position in texture coordinates
uniform float scale;         // Scale factor for height values
uniform vec2 dr;        // 1/texture_resolution

in vec2 TexCoords; // Texture coordinates
out vec4 FragColor; // Fragment color output

void main() {
    // Sample the height at the current fragment position
    float height = texture(field_data, TexCoords).r * scale;

    // Compute the gradients in the x and y directions
    float heightLeft  = texture(field_data, TexCoords + vec2(-dr.x,   0.0)).r * scale;
    float heightRight = texture(field_data, TexCoords + vec2( dr.x,   0.0)).r * scale;
    float heightDown  = texture(field_data, TexCoords + vec2(  0.0, -dr.y)).r * scale;
    float heightUp    = texture(field_data, TexCoords + vec2(  0.0,  dr.y)).r * scale;

    // Compute the surface normal using the gradients
    vec3 normal = normalize(vec3(heightLeft - heightRight, heightDown - heightUp, 2.0));

    // Compute the direction to the light source
    vec2 lightDir2D = normalize(light_pos - TexCoords);
    vec3 lightDir = normalize(vec3(lightDir2D, 1.0));

    // Compute the dot product between the normal and the light direction
    float lightIntensity = dot(normal, lightDir);

    // Clamp the light intensity to the range [0, 1]
    lightIntensity = clamp(lightIntensity, 0.0, 1.0);

    // Output the color based on the light intensity
    FragColor = vec4(vec3(lightIntensity), 1.0);
}