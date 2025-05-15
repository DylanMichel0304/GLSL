#version 330 core
out vec4 FragColor;

in vec3 texCoords;
uniform float alpha;

uniform samplerCube skybox;

void main()
{    
    vec4 texColor = texture(skybox, texCoords);
    FragColor = vec4(texColor.rgb, alpha);
}