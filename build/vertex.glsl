#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec4 aColor;
layout (location = 3) in vec2 aTexCoord;

out vec3 Normal;
out vec3 FragPos;
out vec4 Color;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    // Transform vertex position to clip space
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
    // Pass fragment position in world space for lighting calculations
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    // Transform normal to world space
    Normal = mat3(transpose(inverse(model))) * aNormal;
    
    // Pass through texture coordinates and color
    TexCoord = aTexCoord;
    Color = aColor;
} 