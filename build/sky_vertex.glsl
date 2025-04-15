#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    // Simply pass the position as texture coordinates for the fragment shader
    TexCoords = aPos;
    
    // Transform the position to clip space
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
    // Move sky to maximum depth (z = w)
    gl_Position = gl_Position.xyww;
} 