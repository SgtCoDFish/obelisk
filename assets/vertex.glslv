#version 150 core

in vec2 position;
in vec4 color;
in vec2 texcoord;

out vec2 frag_texcoord;
out vec4 frag_color;

uniform mat4 projTrans;

void main() {
    frag_color = color;
    frag_texcoord = texcoord;
    gl_Position = projTrans * vec4(position, 0.0, 1.0);
}
