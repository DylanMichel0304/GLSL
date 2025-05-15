#version 330 core
layout (location = 0) in vec2 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 camMatrix;

out vec2 TexCoords;

void main() {
    // Map quad coordinates to texture coordinates (0-1)
    TexCoords = aPos + 0.5;
    
    // Position in world space with model matrix for location/size
    vec4 worldPos = model * vec4(aPos, 0.0, 1.0);
    
    // Project using combined camera matrix
    gl_Position = camMatrix * worldPos;
}
