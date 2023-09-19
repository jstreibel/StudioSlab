#version 460 compatibility

layout (location = 0) in vec2 aPos;  // Input 2D coordinate
layout (location = 1) in vec2 aTexCoord;

uniform mat3 transformMatrix;        // 3x3 transformation matrix

out vec2 texCoord;

void main()
{
    texCoord = aTexCoord;
    vec3 pos = transformMatrix * vec3(aSpacePos, 1.0);

    gl_Position = vec4(pos, 1.0);
}
