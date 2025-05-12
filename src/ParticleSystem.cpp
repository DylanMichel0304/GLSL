#include "ParticleSystem.h"
#include <cstdlib>
#include <glm/gtc/matrix_transform.hpp>

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
    shader->Activate();
    glBindVertexArray(VAO);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDepthMask(GL_FALSE);

    // Set camera uniforms
    glUniform3f(glGetUniformLocation(shader->ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
    camera.Matrix(*shader, "camMatrix");

    for (const auto& p : particles) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, p.position);
        model = glm::scale(model, glm::vec3(p.size * 10.0f));  // *10 is fine if intentional

        glUniformMatrix4fv(glGetUniformLocation(shader->ID, "model"), 1, GL_FALSE, &model[0][0]);
        glUniform1f(glGetUniformLocation(shader->ID, "alpha"), p.alpha);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    // 🟩 Restore depth writing
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}
