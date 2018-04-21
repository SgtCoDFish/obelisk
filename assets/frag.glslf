#version 150 core

in vec4 frag_color;
in vec2 frag_texcoord;

out vec4 outColor;

uniform sampler2D tex;

void main() {
    outColor = frag_color * texture(tex, frag_texcoord);
}
