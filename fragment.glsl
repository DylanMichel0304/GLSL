#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;
in float Height;

void main() {
    // Base color from vertex coloring
    vec3 color = ourColor;
    
    // Add subtle variation based on height and texture coordinates
    // Very subtle terrain detail pattern
    float pattern = sin(TexCoord.x * 50.0) * sin(TexCoord.y * 50.0) * 0.01;
    
    // More subtle enhancement based on height
    if (Height > 12.0) {
        // Snowy mountain tops - add a bit of bluish tint to shadows
        float snowPattern = sin(TexCoord.x * 200.0) * sin(TexCoord.y * 200.0) * 0.05;
        color = mix(color, vec3(0.95, 0.95, 1.0), min((Height - 12.0) / 16.0 + snowPattern, 0.8));
    } 
    else if (Height < -3.0) {
        // Water effects - subtle wave pattern
        float waterPattern = sin(TexCoord.x * 20.0 + TexCoord.y * 30.0) * 0.02;
        color += vec3(0.0, waterPattern, waterPattern * 2.0);
    }
    
    // Add the subtle pattern without changing the overall color scheme
    color = mix(color, color + vec3(pattern), 0.1);

    FragColor = vec4(color, 1.0);
} 