

float omni_brightness(vec3 pos, vec3 light_position, vec3 normal)
{
    vec3 surface_to_light = light_position - pos;
    float d = length(surface_to_light);
    float brightness = dot(normal, surface_to_light)/(d*d*d);

    brightness = max(min(brightness,1.0),0.0);

    return brightness;
}

float omni_brightness_no_decay(vec3 pos, vec3 light_position, vec3 normal)
{
    vec3 surface_to_light = light_position - pos;
    float d = length(surface_to_light);
    float brightness = dot(normal, surface_to_light)/d;

    brightness = max(min(brightness,1.0),0.0);

    return brightness;
}

float sun_brightness(vec3 light_direction, vec3 normal)
{
    float brightness = dot(normal, light_direction);

    return max(min(brightness,1.0),0.0);;
}
