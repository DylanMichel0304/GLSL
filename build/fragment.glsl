#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;
in float Height;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightDir;
uniform float dayFactor;
uniform float timeOfDay;

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
    
    // Calculate lighting
    vec3 normal = normalize(Normal);
    
    // Ambient light (varies with day/night)
    vec3 ambientDay = vec3(0.4, 0.4, 0.45);
    vec3 ambientNight = vec3(0.02, 0.02, 0.08);
    vec3 ambient = mix(ambientNight, ambientDay, dayFactor);
    
    // Diffuse lighting
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuseDay = vec3(1.0, 0.9, 0.7);  // Warm sunlight
    vec3 diffuseNight = vec3(0.05, 0.05, 0.15); // Moonlight (bluish)
    vec3 diffuse = mix(diffuseNight, diffuseDay, dayFactor) * diff;
    
    // Sunset/sunrise special coloration
    bool isSunsetOrSunrise = (timeOfDay >= 5.0 && timeOfDay <= 7.0) || 
                             (timeOfDay >= 19.0 && timeOfDay <= 21.0);
    
    if (isSunsetOrSunrise) {
        // Warmer lighting during sunset/sunrise
        float sunsetFactor;
        if (timeOfDay >= 19.0) {
            sunsetFactor = 1.0 - abs(timeOfDay - 20.0) / 1.0; // Peak at 20:00
        } else {
            sunsetFactor = 1.0 - abs(timeOfDay - 6.0) / 1.0;  // Peak at 6:00
        }
        
        vec3 sunsetColor = vec3(0.9, 0.6, 0.3);
        diffuse = mix(diffuse, sunsetColor * diff, sunsetFactor);
    }
    
    // Calculate shadows
    float shadowFactor = 1.0;
    // Simple self-shadowing based on diffuse light angle
    if (diff < 0.1) {
        shadowFactor = mix(0.5, 1.0, diff / 0.1);
    }
    
    // Apply lighting to color
    vec3 lighting = ambient + diffuse * shadowFactor;
    vec3 result = color * lighting;
    
    // Add fog effect - stronger at night
    float distance = length(FragPos);
    float fogDayStart = 80.0;
    float fogDayEnd = 200.0;
    float fogNightStart = 40.0;
    float fogNightEnd = 100.0;
    
    float fogStart = mix(fogNightStart, fogDayStart, dayFactor);
    float fogEnd = mix(fogNightEnd, fogDayEnd, dayFactor);
    
    float fogFactor = clamp((distance - fogStart) / (fogEnd - fogStart), 0.0, 1.0);
    
    vec3 fogColorDay = vec3(0.5, 0.8, 0.9);
    vec3 fogColorNight = vec3(0.05, 0.05, 0.1);
    vec3 fogColor = mix(fogColorNight, fogColorDay, dayFactor);
    
    // Apply fog
    result = mix(result, fogColor, fogFactor);
    
    // Gamma correction
    result = pow(result, vec3(1.0/2.2));

    FragColor = vec4(result, 1.0);
} 