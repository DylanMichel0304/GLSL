#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Collider.h"

inline std::vector<Collider> CreateTreeColliders(const Collider* trunkCollider, const std::vector<glm::vec3>& treePositions) {
    std::vector<Collider> colliders;
    if (!trunkCollider) {
        std::cout << "WARNING: Trunk collider not found for tree, no tree collision will be applied" << std::endl;
        return colliders;
    }

    glm::vec3 originalMin = trunkCollider->getMin();
    glm::vec3 originalMax = trunkCollider->getMax();
    float trunkHeight = originalMax.y - originalMin.y;
    float boxWidth = originalMax.x - originalMin.x;
    float boxDepth = originalMax.z - originalMin.z;
    float trunkRadius = std::min(boxWidth, boxDepth) * 0.1f;

    for (const auto& pos : treePositions) {
        glm::vec3 trunkCenter = glm::vec3(pos.x, pos.y, pos.z);
        colliders.emplace_back(trunkCenter, trunkRadius, trunkHeight);
    }
    return colliders;
}
