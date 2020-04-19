#version 130

in vec4 position;
out vec2 texcoord;

void main() {
    gl_Position = vec4(position.xy, 0.0f, 1.0f);
    texcoord = position.zw;
}
