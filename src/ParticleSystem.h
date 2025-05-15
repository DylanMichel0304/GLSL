#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shaderClass.h"
#include "Particle.h"
#include "Camera.h"

class ParticleSystem {
public:
    std::vector<Particle> particles;
    unsigned int VAO, VBO;
    Shader* shader;
    GLuint textureID;

    ParticleSystem(Shader* shader, GLuint textureID);
    ~ParticleSystem();

    void update(float dt);
    void draw(Camera& camera); // Changed to take Camera&

    void emit(glm::vec3 origin);
};

#endif
