#version 460 compatibility

layout (location = 0) in vec2 aPos;  // Input 2D coordinate
uniform mat3 transformMatrix;        // 3x3 transformation matrix

void main()
{
    vec3 pos = transformMatrix * vec3(aPos, 1.0);
    gl_Position = vec4(pos.xy, 0.0, 1.0);
}
