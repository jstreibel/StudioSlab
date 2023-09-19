#version 460 compatibility

in vec2 fieldCoord;
in vec2 texCoord;

uniform sampler2D fieldData;

layout (location = 0) out vec4 FragColor;

void main() {
    FragColor = texture(fieldData, texCoord);
}
