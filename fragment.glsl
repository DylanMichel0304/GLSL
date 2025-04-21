#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;
in float Height;
in vec3 FragPos;
in vec3 Normal;

// Uniforms for textures
uniform sampler2D sandTexture;
uniform sampler2D grassTexture;
uniform sampler2D rockTexture;
uniform sampler2D waterTexture;

// Uniforms for lighting
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform float ambientStrength;

uniform float time; // For wave animation

void main() {
    // Base color from vertex coloring
    vec3 color = ourColor;
    
    // Sample textures
    vec4 sandColor = texture(sandTexture, TexCoord);
    vec4 grassColor = texture(grassTexture, TexCoord);
    vec4 rockColor = texture(rockTexture, TexCoord);
    vec4 waterColor = texture(waterTexture, TexCoord + vec2(sin(time * 0.1 + TexCoord.x * 10.0) * 0.01, cos(time * 0.1 + TexCoord.y * 10.0) * 0.01));
    
    // Initialize final color
    vec4 finalColor;
    
    // Blend textures based on height
    if (Height < -0.5) {
        // Water
        float waveTime = time * 0.5;
        float wave1 = sin(TexCoord.x * 30.0 + TexCoord.y * 20.0 + waveTime) * 0.02;
        float wave2 = sin(TexCoord.x * 15.0 - TexCoord.y * 25.0 + waveTime * 0.7) * 0.015;
        
        // Mixing water color with waves
        finalColor = waterColor;
        finalColor.rgb += vec3(0.0, wave1, wave2 * 2.0);
        
        // Add shore foam
        if (Height > -1.5) {
            float shoreFoam = sin(TexCoord.x * 100.0 + TexCoord.y * 100.0 + waveTime * 2.0) * 0.05;
            shoreFoam += sin(TexCoord.x * 50.0 - TexCoord.y * 70.0 + waveTime) * 0.05;
            shoreFoam = clamp(shoreFoam + (Height + 1.5) / 1.5 * 0.3, 0.0, 0.6);
            
            finalColor = mix(finalColor, vec4(0.9, 0.95, 1.0, 1.0), shoreFoam);
        }
    } 
    else if (Height < 0.5) {
        // Beach - sand
        finalColor = sandColor;
    }
    else if (Height < 8.0) {
        // Grass with sand blending near shore
        float blendFactor = smoothstep(0.5, 2.0, Height);
        finalColor = mix(sandColor, grassColor, blendFactor);
    }
    else if (Height < 12.0) {
        // Grass to rock transition
        float blendFactor = smoothstep(8.0, 12.0, Height);
        finalColor = mix(grassColor, rockColor, blendFactor);
    }
    else {
        // Mountains - rock with snow on peaks
        float snowFactor = smoothstep(12.0, 18.0, Height);
        finalColor = mix(rockColor, vec4(0.95, 0.95, 1.0, 1.0), snowFactor);
    }
    
    // Lighting calculations
    // Ambient lighting
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular lighting
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
    
    // Special case for water (more reflective)
    if (Height < -0.5) {
        specular *= 2.0; // More reflective water
    }
    
    // Apply lighting
    vec3 lighting = ambient + diffuse + specular;
    finalColor.rgb *= lighting;
    
    // Output final color
    FragColor = finalColor;
} 