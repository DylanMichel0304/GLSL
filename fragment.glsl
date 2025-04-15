#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;
in float Height;

void main() {
    // Base color from vertex coloring
    vec3 color = ourColor;
    
    // Add some variation based on height and texture coordinates
    // Subtle patterns based on texture coordinates
    float pattern = sin(TexCoord.x * 100.0) * sin(TexCoord.y * 100.0) * 0.03;
    
    // Enhance colors based on height
    if (Height > 10.0) {
        // Rocky mountain tops
        color = mix(color, vec3(0.7, 0.7, 0.7), min((Height - 10.0) / 10.0, 0.3));
    } else if (Height < -5.0) {
        // Deeper areas blend toward blue
        color = mix(color, vec3(0.1, 0.3, 0.5), min(abs(Height + 5.0) / 10.0, 0.5));
    }
    
    // Add the subtle pattern
    color += vec3(pattern);

    FragColor = vec4(color, 1.0);
} 