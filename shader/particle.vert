#version 330 core
layout (location = 0) in vec2 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoords;

void main() {
    TexCoords = aPos + 0.5;
    vec4 worldPos = model * vec4(aPos, 0.0, 1.0);
    gl_Position = projection * view * worldPos;
}
