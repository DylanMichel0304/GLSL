#pragma once
#include "Mesh.h"
#include "shaderClass.h"
#include "Camera.h"
#include <glm/glm.hpp>

class Campfire {
public:
    Campfire(glm::vec3 position, float scale = 1.0f);
    ~Campfire();
    void Draw(Camera& camera);
    void Update(float deltaTime);
    
    // Methods to control dynamic scaling
    void SetScale(float newScale);
    float GetScale() const { return scale; }
    
private:
    glm::vec3 position;
    float scale;
    float time;
    
    // Shader for the fire particles
    Shader* shader;
    
    // Geometry for rendering
    GLuint VAO, VBO;
    GLuint numParticles;
    
    // Initialize geometry with vertices
    void initGeometry();
}; 