#version 330 core
out vec4 FragColor;

in float percentTravel_For_Frag;

// Direct fire color from C++ code
uniform vec3 fireColor;

// Define even more intense orange fire color for internal use
const vec3 FIRE_GLOW_COLOR = vec3(1.0, 0.2, 0.0);  // More intense orange-red for center

// Smooth interpolation function with better control
float customSmoothstep(float edge0, float edge1, float x) {
    // Similar to smoothstep but with a more gradual, natural curve
    float t = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
    return t * t * (3.0 - 2.0 * t);
}

void main() 
{
    // Use raw travel percentage - the vertex shader now passes the full range
    float adjustedTravel = percentTravel_For_Frag;
    
    // Fix texture coordinates - OpenGL point sprites use (0,0) at bottom left
    vec2 texCoord = gl_PointCoord;
    
    // Create a circular mask to avoid rectangle shapes
    float distance = length(texCoord - vec2(0.5, 0.5));
    
    // If outside of circle radius, discard the fragment
    if (distance > 0.5) {
        discard;
    }
    
    // Apply circular falloff for softer edges
    float falloff = customSmoothstep(0.5, 0.3, distance);
    
    // Use radial gradient for natural fire look
    vec3 particleColor = mix(
        fireColor,                     // Use the uniform color as base
        FIRE_GLOW_COLOR,               // More intense center
        1.0 - smoothstep(0.0, 0.4, distance * 2.0) // Center is more intense
    );
    
    // Adjust color based on height:
    // - Bottom: Pure fire color
    // - Middle: Transition
    // - Top: More transparent smoke color
    vec3 finalFireColor = particleColor;
    
    if (adjustedTravel > 0.5) {
        // Transition to more muted color for smoke at the top
        float smokeAmount = smoothstep(0.5, 1.0, adjustedTravel);
        finalFireColor = mix(
            finalFireColor,
            vec3(0.8, 0.8, 0.8) * 0.5, // Light gray smoke
            smokeAmount
        );
    }
    
    // Set direct color with falloff-adjusted alpha
    vec4 finalColor = vec4(finalFireColor, (1.0 - adjustedTravel * 0.7) * falloff);
    
    // Create a much more gradual fade-out as particles rise
    float topFade = max(0.0, 1.0 - pow(adjustedTravel, 1.2) * 0.8);
    
    // Extra smoothing at the top portion for extremely gradual dissipation
    if (adjustedTravel > 0.5) {
        float topPortion = (adjustedTravel - 0.5) / 0.5; // 0 to 1 for top half
        float cosineFade = 0.5 * (1.0 + cos(topPortion * 3.14159)); // Cosine curve: 1 → 0
        topFade *= cosineFade;
    }
    
    // Additional random variation in alpha for natural dissipation
    float randomVariation = fract(sin(dot(texCoord, vec2(12.9898, 78.233))) * 43758.5453);
    float noiseAmount = 0.1; // Subtle amount
    
    // Apply all fade factors
    finalColor.a *= topFade * (1.0 - noiseAmount + noiseAmount * randomVariation);
    
    // Boost brightness
    finalColor.rgb = pow(finalColor.rgb, vec3(0.7));
    
    // Discard nearly transparent pixels
    if (finalColor.a < 0.03) {
        discard;
    }
    
    FragColor = finalColor;
} 