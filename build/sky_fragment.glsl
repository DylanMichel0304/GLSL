#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform float timeOfDay;
uniform vec3 lightDir;

// Constants for realistic sky colors
const vec3 SKY_DAY_ZENITH = vec3(0.3, 0.6, 1.0);  
const vec3 SKY_DAY_HORIZON = vec3(0.7, 0.8, 1.0);
const vec3 SKY_NIGHT_ZENITH = vec3(0.02, 0.02, 0.05);
const vec3 SKY_NIGHT_HORIZON = vec3(0.05, 0.05, 0.1);
const vec3 SUN_COLOR = vec3(1.0, 0.9, 0.7);
const vec3 SUNSET_COLOR = vec3(1.0, 0.4, 0.1);

// Noise functions for clouds
float hash(vec3 p) {
    p = fract(p * vec3(443.897, 441.423, 437.195));
    p += dot(p, p.yzx + 19.19);
    return fract((p.x + p.y) * p.z);
}

float noise(vec3 p) {
    vec3 i = floor(p);
    vec3 f = fract(p);
    f = f*f*(3.0-2.0*f);
    
    float n = mix(
        mix(
            mix(hash(i+vec3(0,0,0)), hash(i+vec3(1,0,0)), f.x),
            mix(hash(i+vec3(0,1,0)), hash(i+vec3(1,1,0)), f.x),
            f.y
        ),
        mix(
            mix(hash(i+vec3(0,0,1)), hash(i+vec3(1,0,1)), f.x),
            mix(hash(i+vec3(0,1,1)), hash(i+vec3(1,1,1)), f.x),
            f.y
        ),
        f.z
    );
    
    return n;
}

float fbm(vec3 p) {
    float f = 0.0;
    float weight = 0.5;
    
    for (int i = 0; i < 5; i++) {
        f += weight * noise(p);
        p *= 2.1;
        weight *= 0.5;
    }
    
    return f;
}

// Sun disc and glow effect
float sunDisc(vec3 rayDir, vec3 sunDir, float size) {
    float sunAngle = dot(rayDir, sunDir);
    float disc = smoothstep(cos(size), cos(size*0.5), sunAngle);
    return disc;
}

float sunGlow(vec3 rayDir, vec3 sunDir, float intensity) {
    float angle = dot(rayDir, sunDir);
    return pow(max(0.0, angle), intensity);
}

void main() {
    // Normalized view direction
    vec3 rayDir = normalize(TexCoords);
    
    // Calculate altitude for horizon blending
    float altitude = rayDir.y;
    
    // Day/night factor based on time of day
    float dayFactor;
    float sunsetFactor = 0.0;
    
    if (timeOfDay > 7.0 && timeOfDay < 19.0) {
        // Full daylight
        dayFactor = 1.0;
        
        // Sunset transition
        if (timeOfDay > 17.0) {
            sunsetFactor = (timeOfDay - 17.0) / 2.0;
        } else if (timeOfDay < 9.0) {
            sunsetFactor = (9.0 - timeOfDay) / 2.0;
        }
    } else if (timeOfDay >= 19.0 && timeOfDay <= 21.0) {
        // Dusk transition
        dayFactor = 1.0 - (timeOfDay - 19.0) / 2.0;
        sunsetFactor = 1.0 - abs(timeOfDay - 20.0) / 1.0; // Peak at 20:00
    } else if (timeOfDay >= 5.0 && timeOfDay <= 7.0) {
        // Dawn transition
        dayFactor = (timeOfDay - 5.0) / 2.0;
        sunsetFactor = 1.0 - abs(timeOfDay - 6.0) / 1.0; // Peak at 6:00
    } else {
        // Full night
        dayFactor = 0.0;
    }
    
    // Calculate horizon gradient
    float horizonGradient = 1.0 - pow(altitude, 1.5);
    
    // Mix day/night sky colors
    vec3 dayColor = mix(SKY_DAY_ZENITH, SKY_DAY_HORIZON, horizonGradient);
    vec3 nightColor = mix(SKY_NIGHT_ZENITH, SKY_NIGHT_HORIZON, horizonGradient);
    vec3 skyColor = mix(nightColor, dayColor, dayFactor);
    
    // Add sunset/sunrise effect
    if (sunsetFactor > 0.0) {
        // Calculate sun direction contribution to sunset
        float sunContrib = max(0.0, dot(rayDir, vec3(lightDir.x, 0.0, lightDir.z)));
        vec3 sunsetColor = mix(SUNSET_COLOR, SUN_COLOR, sunContrib);
        
        // Apply sunset coloration near horizon
        float sunsetBand = pow(1.0 - abs(altitude), 4.0);
        skyColor = mix(skyColor, sunsetColor, sunsetFactor * sunsetBand);
    }
    
    // Add sun or moon based on time
    vec3 celestialColor = vec3(0.0);
    
    if (dayFactor > 0.05) {
        // Sun disc
        float disc = sunDisc(rayDir, lightDir, 0.03);
        
        // Sun glow
        float glow = sunGlow(rayDir, lightDir, 32.0) * 0.25;
        float haze = sunGlow(rayDir, lightDir, 8.0) * 0.15;
        
        // Sun color based on height (more red when near horizon)
        vec3 sunColor = mix(SUNSET_COLOR, SUN_COLOR, min(1.0, lightDir.y * 2.0 + 0.5));
        
        celestialColor += sunColor * disc * 3.0;
        celestialColor += sunColor * glow;
        celestialColor += vec3(1.0, 0.8, 0.5) * haze * dayFactor;
    } else {
        // Moon at night (opposite direction)
        vec3 moonDir = -lightDir;
        float moonDisc = sunDisc(rayDir, moonDir, 0.015);
        float moonGlow = sunGlow(rayDir, moonDir, 16.0) * 0.1;
        
        celestialColor += vec3(0.8, 0.85, 1.0) * moonDisc;
        celestialColor += vec3(0.5, 0.5, 0.8) * moonGlow;
    }
    
    // Add clouds
    if (dayFactor > 0.0 && altitude > 0.0) {
        // Project sky dome to cloud plane
        float cloudPlaneHeight = 20.0;
        float t = cloudPlaneHeight / max(0.001, rayDir.y);
        vec3 cloudPos = vec3(rayDir.x * t, cloudPlaneHeight, rayDir.z * t) * 0.01;
        
        // Animate clouds slowly
        cloudPos.xz += vec2(timeOfDay * 0.001, timeOfDay * 0.0005);
        
        // Generate cloud patterns with multiple layers
        float cloudDensity = fbm(cloudPos * 2.0);
        float cloudDetail = fbm(cloudPos * 8.0 + vec3(0.5));
        
        // Cloud mask with height falloff
        float heightFalloff = 1.0 - pow(altitude, 2.0);
        float cloudMask = smoothstep(0.4, 0.6, cloudDensity) * heightFalloff;
        
        // Cloud lighting
        float sunDot = max(0.0, dot(vec3(0.0, 1.0, 0.0), lightDir));
        float brightness = mix(0.3, 1.0, sunDot);
        
        // Cloud color
        vec3 cloudDayColor = vec3(1.0);
        vec3 cloudNightColor = vec3(0.2, 0.2, 0.3);
        vec3 cloudSunsetColor = vec3(1.0, 0.6, 0.3);
        
        vec3 baseCloudColor = mix(cloudNightColor, cloudDayColor, dayFactor);
        baseCloudColor = mix(baseCloudColor, cloudSunsetColor, sunsetFactor);
        
        vec3 cloudColor = baseCloudColor * brightness;
        
        // Apply clouds
        skyColor = mix(skyColor, cloudColor, cloudMask * dayFactor);
    }
    
    // Add stars at night
    if (dayFactor < 0.2) {
        float starVisibility = 1.0 - dayFactor / 0.2;
        float starValue = hash(rayDir * 500.0);
        
        if (starValue > 0.998) {
            float starBrightness = (starValue - 0.998) * 200.0;
            float twinkle = sin(timeOfDay * 10.0 + starValue * 100.0) * 0.5 + 0.5;
            
            vec3 starColor = vec3(0.9, 0.9, 1.0) * starBrightness * twinkle * starVisibility;
            skyColor += starColor;
        }
    }
    
    // Combine sky and celestial objects
    vec3 finalColor = skyColor + celestialColor;
    
    // Apply atmospheric scattering at horizon
    if (dayFactor > 0.0) {
        float scatter = 1.0 - exp(-0.3 / max(0.0001, altitude));
        finalColor = mix(finalColor, vec3(0.7, 0.8, 1.0), scatter * 0.5 * dayFactor);
    }
    
    // Gamma correction
    finalColor = pow(finalColor, vec3(1.0/2.2));
    
    FragColor = vec4(finalColor, 1.0);
} 