#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;
out float Height;
out vec3 FragPos; // Send fragment position for lighting calculation
out vec3 Normal;  // Send normal for lighting calculation

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0)); // World space position
    
    // Calculate normal - for simple terrain, just use up vector
    // In a real implementation, you would calculate this from neighboring vertices
    Normal = mat3(transpose(inverse(model))) * vec3(0.0, 1.0, 0.0);
    
    ourColor = aColor;
    TexCoord = aTexCoord * 20.0; // Scale texture coordinates for terrain
    Height = aPos.y; // Pass height to fragment shader for additional effects
} 