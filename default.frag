#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;

// Imports the current position from the Vertex Shader
in vec3 crntPos;
// Imports the normal from the Vertex Shader
in vec3 Normal;
// Imports the color from the Vertex Shader
in vec3 color;
// Imports the texture coordinates from the Vertex Shader
in vec2 texCoord;

// Material struct to hold properties from MTL file
struct Material {
    vec3 ambient;  // Ka
    vec3 diffuse;  // Kd
    vec3 specular; // Ks
    float shininess; // Ns
};

// Gets the Texture Units from the main function
uniform sampler2D diffuse0;
uniform sampler2D specular0;
// Gets the color of the light from the main function
uniform vec4 lightColor;
// Gets the position of the light from the main function
uniform vec3 lightPos;
// Gets the position of the camera from the main function
uniform vec3 camPos;
// Material properties
uniform Material material;

// Tiling factor for the textures
const float tiling = 1.0; // Reset to 1.0 to respect texture coords in model

// Sample texture with tiling
vec4 sampleDiffuseTexture() {
    vec4 texColor = texture(diffuse0, texCoord * tiling);
    // Mix with material's diffuse color
    if (texColor.a == 0.0) {
        return vec4(material.diffuse, 1.0);
    }
    
    // If has texture, multiply by material diffuse
    return texColor * vec4(material.diffuse, 1.0);
}

// Sample specular texture with tiling
float sampleSpecularTexture() {
    // Use material's specular if no texture
    float texSpec = texture(specular0, texCoord * tiling).r;
    if (texSpec == 0.0) {
        return length(material.specular);
    }
    return texSpec * length(material.specular);
}

vec4 pointLight()
{	
	// used in two variables so I calculate it here to not have to do it twice
	vec3 lightVec = lightPos - crntPos;

	// intensity of light with respect to distance
	float dist = length(lightVec);
	float a = 3.0;   // Atténuation quadratique
	float b = 0.7;   // Atténuation linéaire
	float inten = 1.0f / (a * dist * dist + b * dist + 1.0f);

	// ambient lighting - use material ambient
	float ambient = length(material.ambient) * 0.5;
	if (ambient < 0.2) ambient = 0.2; // Minimum ambient

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightVec);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting - use material shininess
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), material.shininess);
	float specular = specAmount * specularLight;

	return (sampleDiffuseTexture() * (diffuse * inten + ambient) + sampleSpecularTexture() * specular * inten) * lightColor;
}

vec4 direcLight()
{
	// ambient lighting - use material ambient
	float ambient = length(material.ambient) * 0.5;
	if (ambient < 0.2) ambient = 0.2; // Minimum ambient

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(vec3(1.0f, 1.0f, 0.0f));
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting - use material shininess
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), material.shininess);
	float specular = specAmount * specularLight;

	return (sampleDiffuseTexture() * (diffuse + ambient) + sampleSpecularTexture() * specular) * lightColor;
}

vec4 spotLight()
{
	// controls how big the area that is lit up is
	float outerCone = 0.90f;
	float innerCone = 0.95f;

	// ambient lighting - use material ambient
	float ambient = length(material.ambient) * 0.5;
	if (ambient < 0.2) ambient = 0.2; // Minimum ambient

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPos - crntPos);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting - use material shininess
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), material.shininess);
	float specular = specAmount * specularLight;

	// calculates the intensity of the crntPos based on its angle to the center of the light cone
	float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDirection);
	float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);

	return (sampleDiffuseTexture() * (diffuse * inten + ambient) + sampleSpecularTexture() * specular * inten) * lightColor;
}

float linearrizeDepth(float depth)
{
	// converts the depth value to a linear value
	float z = depth * 2.0f - 1.0f;
	float zNear = 0.1f;
	float zFar = 100.0f;
	return (2.0f * zNear * zFar) / (zFar + zNear - z * (zFar - zNear));
}

void main()
{
	// outputs final color
	FragColor = pointLight();
}