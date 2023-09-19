#version 460 compatibility

layout (location = 0) in vec2 vertex;
layout (location = 1) in vec2 tex_coord;

uniform mat3 transformMatrix;        // 3x3 transformation matrix

out vec2 TexCoord;

void main()
{
    TexCoord = tex_coord;
    vec3 pos = transformMatrix*vec3(vertex, 1.0);
    gl_Position       = vec4(pos.xy, 0.0, 1.0);
}