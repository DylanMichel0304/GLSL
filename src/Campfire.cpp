#include "Campfire.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdlib.h>
#include "stb_image.h"

Campfire::Campfire(glm::vec3 position, float scale) : position(position), scale(scale), time(0.0f) {
    // Load shader
    shader = new Shader("shader/campfire.vert", "shader/campfire.frag");
    
    // Initialize particle geometry
    numParticles = 10000; // 100x100 grid of particles
    initGeometry();
    
    std::cout << "Campfire created at position: " << position.x << ", " 
              << position.y << ", " << position.z << std::endl;
}

Campfire::~Campfire() {
    // Cleanup resources
    delete shader;
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Campfire::initGeometry() {
    // Generate vertices for particles
    GLfloat* vertices = new GLfloat[numParticles * 3 *2]; 
    int index = 0;
    int particlesGenerated = 0;
    
    // Create a circular distribution of starting positions for particles
    while (particlesGenerated < numParticles) {
        // Generate random point in a square
        float rx = ((float)rand() / RAND_MAX) * 2.0f - 1.0f; // -1 to 1
        float rz = ((float)rand() / RAND_MAX) * 2.0f - 1.0f; // -1 to 1
        
        // Calculate distance from center
        float distFromCenter = sqrt(rx*rx + rz*rz);
        
        // Only accept points within the unit circle (distance < 1)
        if (distFromCenter <= 1.0f) {
            // Apply the spread factor to control the size
            float spread = 0.6f;
            vertices[index++] = rx * spread;
            vertices[index++] = 0.0f;  // y: start at ground level
            vertices[index++] = rz * spread;
            
            // Add density towards the center for more realistic fire shape
            if (distFromCenter < 0.6f) {
                // Add a second point nearby for denser center
                float jitterX = ((float)rand() / RAND_MAX) * 0.1f - 0.05f;
                float jitterZ = ((float)rand() / RAND_MAX) * 0.1f - 0.05f;
                
                vertices[index++] = rx * spread + jitterX;
                vertices[index++] = 0.0f;
                vertices[index++] = rz * spread + jitterZ;
                
                particlesGenerated++;
                if (particlesGenerated >= numParticles) break;
            }
            
            particlesGenerated++;
        }
    }

    // Create and bind VAO and VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, index * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    
    // Set up vertex attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // Modern OpenGL (3.3+) handles point sprites automatically
    // Just need to enable the program point size
    glEnable(GL_PROGRAM_POINT_SIZE);
    
    std::cout << "Particle system initialized with " << numParticles << " particles" << std::endl;
    
    // Cleanup temporary data
    delete[] vertices;
}

void Campfire::Update(float deltaTime) {
    time += deltaTime;
}

// Add new method to set the scale dynamically
void Campfire::SetScale(float newScale) {
    scale = newScale;
}

void Campfire::Draw(Camera& camera) {
    // Enable blending for particles
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    
    // Use additive blending for fire particles
    // This creates a more realistic fire effect by adding light
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    
    // Activate shader and set uniforms
    shader->Activate();
    
    // Set time uniform for animation
    GLuint timeLocation = glGetUniformLocation(shader->ID, "time");
    glUniform1f(timeLocation, time);
    
    // Set model-view-projection matrices
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(scale));
    
    // Calculate view matrix from camera
    glm::mat4 view = glm::lookAt(camera.Position, camera.Position + camera.Orientation, camera.Up);
    glm::mat4 modelView = view * model;
    
    // Set modelview and projection matrices
    GLuint modelViewLocation = glGetUniformLocation(shader->ID, "ModelView");
    glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, glm::value_ptr(modelView));
    
    // Use camera's projection matrix
    camera.updateMatrix(45.0f, 0.1f, 100.0f);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)camera.width / camera.height, 0.1f, 100.0f);
    GLuint projectionLocation = glGetUniformLocation(shader->ID, "Projection");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
    
    // Set useTexture flag to 0 to use pure colors instead of textures
    GLuint useTextureLocation = glGetUniformLocation(shader->ID, "useTexture");
    glUniform1i(useTextureLocation, 0);  // 0 = don't use texture, use pure color
    
    // Set a pure orange color directly for fire particles
    GLuint fireColorLocation = glGetUniformLocation(shader->ID, "fireColor");
    glUniform3f(fireColorLocation, 1.0f, 0.5f, 0.05f); // Pure orange
    
    // Draw particles
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, numParticles);
    glBindVertexArray(0);
    
    // Clean up state
    glUseProgram(0);
    
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
} 