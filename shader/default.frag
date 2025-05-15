#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec2 texCoord;
in vec3 crntPos;

uniform sampler2D tex0;            // Diffuse texture
uniform sampler2D tex1;            // Specular map (can be empty or white)
uniform samplerCube cubemapSampler;

uniform float textureTiling;
uniform vec3 camPos;
uniform float reflectivity;        // <--- Ajouté : 0.0 = pas de reflet, 1.0 = miroir

#define MAX_LIGHTS 10

struct Light {
    vec3 position;
    vec3 direction;
    vec4 color;
    int type;
};

uniform Light lights[MAX_LIGHTS];
uniform int lightCount;

void main()
{
    vec3 norm = normalize(Normal);
    vec4 texColor = texture(tex0, texCoord * textureTiling);
    vec4 specMap = texture(tex1, texCoord * textureTiling);
    vec3 viewDir = normalize(camPos - crntPos);

    vec4 finalColor = vec4(0.0);

    for (int i = 0; i < lightCount; i++)
    {
        Light light = lights[i];
        vec3 lightDir;
        float attenuation = 1.0;
        float diff = 0.0;
        float spec = 0.0;
        float ambientStrength = 0.2;
        float specularStrength = 0.5;
        float intensity = 1.0;

        if (light.type == 0)
        {
            lightDir = normalize(-light.direction);
            intensity = 0.3;
        }
        else
        {
            lightDir = normalize(light.position - crntPos);
            float dist = length(light.position - crntPos);
            attenuation = 1.0 / (1.0 + 0.2 * dist + 0.032 * dist * dist);
        }

        diff = max(dot(norm, lightDir), 0.0);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(norm, halfwayDir), 0.0), 16.0);

        if (light.type == 2)
        {
            float theta = dot(lightDir, normalize(-light.direction));
            float outer = 0.7;
            float inner = 0.85;
            float epsilon = inner - outer;
            float spotIntensity = clamp((theta - outer) / epsilon, 0.0, 1.0);
            diff *= spotIntensity;
            spec *= spotIntensity;
            attenuation *= spotIntensity;
        }

        vec3 ambient = ambientStrength * vec3(light.color) * intensity * 2.0;
        vec3 diffuse = diff * vec3(light.color) * intensity * 2.0;
        vec3 specular = specularStrength * spec * vec3(light.color) * specMap.r * intensity * 2.0;

        finalColor += vec4(ambient + diffuse + specular, 1.0) * texColor * attenuation;
    }

    // --- REFLECTION ENVIRONMENT MAPPING ---
    vec3 reflectedDir = reflect(-viewDir, norm);
    vec3 reflection = texture(cubemapSampler, reflectedDir).rgb;

    // Mélange avec la couleur calculée
    finalColor.rgb = mix(finalColor.rgb, reflection, reflectivity);

    FragColor = finalColor;
}
