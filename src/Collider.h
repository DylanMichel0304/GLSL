#pragma once
#include <glm/glm.hpp>

struct Collider {
    glm::vec3 min;
    glm::vec3 max;

    Collider() = default;
    Collider(const glm::vec3& minPoint, const glm::vec3& maxPoint)
        : min(minPoint), max(maxPoint) {}

    bool Intersects(const Collider& other) const {
        return (min.x <= other.max.x && max.x >= other.min.x) &&
               (min.y <= other.max.y && max.y >= other.min.y) &&
               (min.z <= other.max.z && max.z >= other.min.z);
    }

    glm::vec3 Center() const {
        return (min + max) * 0.5f;
    }
};
