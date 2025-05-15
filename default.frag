#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec2 texCoord;
in vec3 crntPos;

// Définir une Material
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

// Définir une Light
#define MAX_LIGHTS 10

struct Light {
    vec3 position;  
    vec3 direction;
    vec4 color;
    int type; // 0 = Directional, 1 = Point, 2 = Spot
};

// Uniforms
uniform Material material;
uniform Light lights[MAX_LIGHTS];
uniform int lightCount;
uniform vec3 camPos;

void main()
{
    vec3 norm = normalize(Normal);
    vec4 texColor = texture(material.diffuse, texCoord);
    vec4 specMap = texture(material.specular, texCoord);

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

        if (light.type == 0) // Directional
        {
            lightDir = normalize(-light.direction);
        }
        else if (light.type == 1 || light.type == 2) // Point ou Spot
        {
            lightDir = normalize(light.position - crntPos);

            // Attenuation
            float dist = length(light.position - crntPos);
            float constant = 1.0;
            float linear = 0.7;
            float quadratic = 1.8;
            attenuation = 1.0 / (constant + linear * dist + quadratic * (dist * dist));
        }

        // Diffuse
        diff = max(dot(norm, lightDir), 0.0);

        // Specular
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);

        // Spot attenuation
        if (light.type == 2)
        {
            float theta = dot(lightDir, normalize(-light.direction));
            float outerCutOff = 0.85;
            float innerCutOff = 0.95;
            float epsilon = innerCutOff - outerCutOff;
            float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);
            diff *= intensity;
            spec *= intensity;
            attenuation *= intensity;
        }

        vec3 ambient = ambientStrength * vec3(light.color);
        vec3 diffuse = diff * vec3(light.color);
        vec3 specular = specularStrength * spec * vec3(light.color) * specMap.r;

        finalColor += vec4(ambient + diffuse + specular, 1.0) * texColor * attenuation;
    }

    FragColor = finalColor;
}
