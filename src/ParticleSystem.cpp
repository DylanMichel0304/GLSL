#include "ParticleSystem.h"
#include <cstdlib>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

ParticleSystem::ParticleSystem(Shader* shader, GLuint textureID)
    : shader(shader), textureID(textureID)
{
    float quad[] = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
        -0.5f,  0.5f,
         0.5f,  0.5f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    
    std::cout << "ParticleSystem created with texture ID: " << textureID << std::endl;
}

ParticleSystem::~ParticleSystem() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void ParticleSystem::emit(glm::vec3 origin) {
    glm::vec3 velocity(
        (rand() % 100 - 50) / 100.0f,
        (rand() % 100) / 100.0f + 1.0f,
        (rand() % 100 - 50) / 100.0f
    );
    particles.emplace_back(origin, velocity, 1.5f, 0.5f);
}

void ParticleSystem::update(float dt) {
    for (auto it = particles.begin(); it != particles.end();) {
        it->life -= dt;
        if (it->life <= 0.0f) {
            it = particles.erase(it);
        } else {
            it->position += it->velocity * dt;
            it->alpha = it->life / 1.5f;
            ++it;
        }
    }
}

void ParticleSystem::draw(Camera& camera) {
    if (particles.empty()) {
        return; // Don't bother drawing if there are no particles
    }

    shader->Activate();
    glBindVertexArray(VAO);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Properly set up blending for particle transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Disable depth writing but keep depth testing
    glDepthMask(GL_FALSE);

    // Set camera uniforms
    glUniform3f(glGetUniformLocation(shader->ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
    camera.Matrix(*shader, "camMatrix");

    // Billboard calculation - ensure particles always face the camera
    glm::vec3 cameraRight = glm::normalize(glm::cross(camera.Orientation, camera.Up));
    glm::vec3 cameraUp = glm::normalize(glm::cross(camera.Orientation, -cameraRight));

    int drawnCount = 0;
    for (const auto& p : particles) {
        // Create billboard model matrix
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, p.position);
        
        // Make the model face the camera (billboard)
        model[0][0] = cameraRight.x; model[1][0] = cameraRight.y; model[2][0] = cameraRight.z;
        model[0][1] = cameraUp.x;    model[1][1] = cameraUp.y;    model[2][1] = cameraUp.z;
        model[0][2] = camera.Orientation.x; model[1][2] = camera.Orientation.y; model[2][2] = camera.Orientation.z;
        
        // Scale the particle
        model = glm::scale(model, glm::vec3(p.size * 20.0f));  // Increased from 10.0f to 20.0f

        glUniformMatrix4fv(glGetUniformLocation(shader->ID, "model"), 1, GL_FALSE, &model[0][0]);
        glUniform1f(glGetUniformLocation(shader->ID, "alpha"), p.alpha);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        drawnCount++;
    }

    // Occasionally show debugging info
    if (rand() % 1000 == 0) {
        std::cout << "Drew " << drawnCount << " particles" << std::endl;
    }

    // Restore depth writing
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}
