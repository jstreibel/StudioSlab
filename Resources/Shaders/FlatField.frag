#version 460 compatibility

in vec2 fieldCoord;
in vec2 texCoord;

uniform sampler2D fieldData;

//layout (location = 0) out vec4 FragColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(1.0, 0.0, 0.0, 1.0); // texture(fieldData, texCoord);
}
