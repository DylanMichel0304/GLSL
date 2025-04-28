#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec2 texCoord;
in vec3 crntPos;

// Textures
uniform sampler2D tex0; // diffuse map
uniform sampler2D tex1; // specular map

float outerCutOff = cos(radians(20.0));
float innerCutOff = cos(radians(15.0));


// Camera
uniform vec3 camPos;

// Définition de la lumière
#define MAX_LIGHTS 10

struct Light {
    vec3 position;  // Pour point light ou spot
    vec3 direction; // Pour directionnelle ou spot
    vec4 color;
    int type; // 0 = Directional, 1 = Point, 2 = Spot
};

uniform Light lights[MAX_LIGHTS];
uniform int lightCount;

void main()
{
    vec3 norm = normalize(Normal);
    vec4 texColor = texture(tex0, texCoord);
    vec4 specMap = texture(tex1, texCoord);

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

            // Calcul de l'atténuation pour point lights et spotlights
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
        spec = pow(max(dot(norm, halfwayDir), 0.0), 16.0);

        // Spotlight : limiter l'angle
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
