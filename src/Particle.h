#ifndef PARTICLE_H
#define PARTICLE_H

#include <glm/glm.hpp>

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    float life;
    float size;
    float alpha;

    Particle(glm::vec3 pos, glm::vec3 vel, float lifetime, float s)
        : position(pos), velocity(vel), life(lifetime), size(s), alpha(1.0f) {}
};

#endif
