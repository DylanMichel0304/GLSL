#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D smokeTexture;
uniform float alpha;

void main() {
    vec4 color = texture(smokeTexture, TexCoords);
    color.a *= alpha;
    if (color.a < 0.05) discard;
    FragColor = color;
}
