#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D smokeTexture;
uniform float alpha;

void main() {
    // Sample texture
    vec4 color = texture(smokeTexture, TexCoords);
    
    // Apply alpha from uniform
    color.a *= alpha;
    
    // Alpha test to avoid processing fully transparent fragments
    if (color.a < 0.01) discard;
    
    // Output color
    FragColor = color;
}
