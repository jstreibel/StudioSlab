#version 330

in vec2 pos;

out vec4 Position;

void main() {
    Position = vec4(pos, 0.0, 1.0);
}
