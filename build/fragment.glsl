#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec4 Color;
in vec2 TexCoord;

uniform sampler2D rockTexture;
uniform sampler2D grassTexture;
uniform sampler2D waterTexture;
uniform sampler2D sandTexture;
uniform sampler2D snowTexture;

// Fog parameters
uniform float fogDensity;
uniform vec3 fogColor;
uniform float fogStart;
uniform float fogEnd;

// Sky color for reflections
uniform vec3 skyColor;

// Camera position for reflections and fog
uniform vec3 cameraPos;

// Time of day
uniform float time;

// Calculate height-based procedural normal for water
vec3 waterNormal(vec2 uv, float height) {
    // Create procedural normal based on wave displacement
    float waveHeight = sin(uv.x * 20.0 + time * 2.0) * 0.05 + 
                      cos(uv.y * 20.0 + time * 1.5) * 0.05;
    vec3 tangent = normalize(vec3(1.0, sin(uv.x * 20.0 + time * 2.0) * 0.5, 0.0));
    vec3 bitangent = normalize(vec3(0.0, cos(uv.y * 20.0 + time * 1.5) * 0.5, 1.0));
    return normalize(cross(tangent, bitangent));
}

// Fresnel approximation for water
float fresnel(vec3 normal, vec3 viewDir, float power) {
    float fresnelTerm = max(0.0, 1.0 - dot(normal, viewDir));
    return pow(fresnelTerm, power);
}

void main() {
    // Determine texture and blending based on height
    float height = FragPos.y;
    
    // Get view direction for reflections and fog
    vec3 viewDir = normalize(cameraPos - FragPos);
    
    // Terrain texturing
    vec4 terrainColor;
    vec3 terrainNormal = normalize(Normal);
    
    if (height < 5.0) { // Water
        // Get procedural normal for water
        vec3 waterNorm = waterNormal(TexCoord, height);
        terrainNormal = normalize(mix(terrainNormal, waterNorm, 0.7));
        
        // Sample water texture
        vec4 water = texture(waterTexture, TexCoord + vec2(sin(time * 0.5 + TexCoord.y * 10.0) * 0.01, 
                                                         cos(time * 0.4 + TexCoord.x * 10.0) * 0.01));
        
        // Add fresnel effect for reflections
        float fresnelFactor = fresnel(terrainNormal, viewDir, 5.0);
        
        // Mix water color with reflections and underwater color
        terrainColor = mix(water * vec4(0.0, 0.2, 0.5, 1.0), 
                          vec4(skyColor * 0.8, 1.0), 
                          fresnelFactor * 0.7);
        
        // Add wave highlights
        float highlight = pow(max(0.0, dot(reflect(-vec3(0.0, 1.0, 0.0), terrainNormal), viewDir)), 32.0);
        terrainColor += vec4(highlight, highlight, highlight, 0.0) * 0.3;
    }
    else if (height < 7.0) { // Beach/Sand
        vec4 sand = texture(sandTexture, TexCoord);
        float blend = smoothstep(5.0, 7.0, height);
        
        // Blend with water at the shore
        vec4 water = texture(waterTexture, TexCoord + vec2(sin(time * 0.5) * 0.01, cos(time * 0.4) * 0.01));
        terrainColor = mix(water * vec4(0.0, 0.2, 0.5, 1.0), sand * vec4(1.0, 0.9, 0.7, 1.0), blend);
    }
    else if (height < 20.0) { // Grass
        vec4 grass = texture(grassTexture, TexCoord);
        vec4 sand = texture(sandTexture, TexCoord);
        float blend = smoothstep(7.0, 10.0, height);
        terrainColor = mix(sand * vec4(1.0, 0.9, 0.7, 1.0), grass * vec4(0.3, 0.7, 0.3, 1.0), blend);
    }
    else if (height < 35.0) { // Rock
        vec4 grass = texture(grassTexture, TexCoord);
        vec4 rock = texture(rockTexture, TexCoord);
        float blend = smoothstep(20.0, 25.0, height);
        terrainColor = mix(grass * vec4(0.3, 0.7, 0.3, 1.0), rock * vec4(0.5, 0.5, 0.5, 1.0), blend);
    }
    else { // Snow
        vec4 rock = texture(rockTexture, TexCoord);
        vec4 snow = texture(snowTexture, TexCoord);
        float blend = smoothstep(35.0, 40.0, height);
        terrainColor = mix(rock * vec4(0.5, 0.5, 0.5, 1.0), snow, blend);
    }
    
    // Basic lighting calculation
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3)); // Light direction
    
    // Ambient light component - varies by time of day
    vec3 ambient = terrainColor.rgb * 0.3;
    
    // Diffuse component
    float diff = max(dot(terrainNormal, lightDir), 0.0);
    vec3 diffuse = diff * terrainColor.rgb;
    
    // Specular component
    vec3 reflectDir = reflect(-lightDir, terrainNormal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * vec3(0.2);
    
    // Final color with lighting
    vec3 result = ambient + diffuse + specular;
    
    // Apply fog effect
    float distanceToCamera = length(cameraPos - FragPos);
    float fogFactor = 1.0 - exp(-fogDensity * distanceToCamera);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    
    // Apply distance-based fog
    result = mix(result, fogColor, fogFactor);
    
    // Apply height-based fog for distant mountains
    if (height > 20.0) {
        float heightFog = smoothstep(0.0, 1.0, (height - 20.0) / 30.0) * 0.5;
        result = mix(result, fogColor, min(heightFog * fogFactor * 2.0, 0.8));
    }
    
    FragColor = vec4(result, 1.0);
} 