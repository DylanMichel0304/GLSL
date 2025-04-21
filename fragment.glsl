#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;
in float Height;

uniform float time; // Add time uniform for wave animation

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
    else if (Height < -0.5) {
        // Enhanced water effects with animated waves
        // Create more interesting wave pattern with time animation
        float waveTime = time * 0.5; // Slow the animation down a bit
        
        // Multiple overlapping wave patterns at different frequencies and directions
        float wave1 = sin(TexCoord.x * 30.0 + TexCoord.y * 20.0 + waveTime) * 0.02;
        float wave2 = sin(TexCoord.x * 15.0 - TexCoord.y * 25.0 + waveTime * 0.7) * 0.015;
        float wave3 = sin(TexCoord.x * 5.0 + TexCoord.y * 5.0 + waveTime * 0.3) * 0.01;
        
        // Combine waves
        float waterEffect = wave1 + wave2 + wave3;
        
        // Apply wave highlight color based on depth
        vec3 deepWaterColor = vec3(0.0, 0.1, 0.4); // Deep water (dark blue)
        vec3 shallowWaterColor = vec3(0.0, 0.5, 0.8); // Shallow water (lighter blue)
        vec3 highlightColor = vec3(0.8, 0.9, 1.0); // Wave highlight (white/light blue)
        
        // Mix colors based on depth and wave height
        float depthFactor = clamp((Height + 5.0) / 5.0, 0.0, 1.0); // 0.0 = deep, 1.0 = shallow
        vec3 baseWaterColor = mix(deepWaterColor, shallowWaterColor, depthFactor);
        
        // Add highlights to wave peaks
        color = mix(baseWaterColor, highlightColor, clamp(waterEffect * 5.0, 0.0, 0.3));
        
        // Add subtle transparency/foam near the shore
        if (Height > -1.5) {
            // Near shore foam effect
            float shoreFoam = sin(TexCoord.x * 100.0 + TexCoord.y * 100.0 + waveTime * 2.0) * 0.05;
            shoreFoam += sin(TexCoord.x * 50.0 - TexCoord.y * 70.0 + waveTime) * 0.05;
            shoreFoam = clamp(shoreFoam + (Height + 1.5) / 1.5 * 0.3, 0.0, 0.6);
            
            // Mix in the shore foam
            color = mix(color, vec3(0.9, 0.95, 1.0), shoreFoam);
        }
    }
    
    // Add the subtle pattern without changing the overall color scheme
    color = mix(color, color + vec3(pattern), 0.1);

    FragColor = vec4(color, 1.0);
} 