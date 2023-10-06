

float lighting(vec3 pos, vec3 light_position, vec3 normal)
{
    vec3 surface_to_light = normalize(light_position - pos);
    float brightness = dot(normal, surface_to_light);

    brightness = max(min(brightness,1.0),0.0);

    return brightness;
}
