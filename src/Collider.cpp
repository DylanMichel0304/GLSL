#include "Collider.h"
#include <iostream>
#include <algorithm>

Collider::Collider(const glm::vec3& min, const glm::vec3& max) 
    : type(ColliderType::BOX), min(min), max(max), center(), radius(), height() {
}

Collider::Collider(const glm::vec3& baseCenter, float radius, float height)
    : type(ColliderType::CYLINDER), radius(radius), height(height), center(baseCenter), min(), max() {
    // Calculate the bounding box for the cylinder (for broad phase collision)
    min = glm::vec3(center.x - radius, center.y, center.z - radius);
    max = glm::vec3(center.x + radius, center.y + height, center.z + radius);
}

glm::vec3 Collider::Center() const {
    if (type == ColliderType::BOX) {
        return (min + max) * 0.5f;
    } else {
        // For cylinder, return the center point at half height
        return glm::vec3(center.x, center.y + height * 0.5f, center.z);
    }
}

bool Collider::Intersects(const Collider& other) const {
    // If either collider is a cylinder, use the specialized intersection check
    if (type == ColliderType::CYLINDER || other.type == ColliderType::CYLINDER) {
        return CylinderIntersect(other);
    }
    
    // Default box-box intersection check
    return !(max.x < other.min.x || min.x > other.max.x ||
             max.y < other.min.y || min.y > other.max.y ||
             max.z < other.min.z || min.z > other.max.z);
}

bool Collider::CylinderIntersect(const Collider& other) const {
    // Handle cylinder-to-cylinder and cylinder-to-box intersections
    
    if (type == ColliderType::CYLINDER && other.type == ColliderType::CYLINDER) {
        // Cylinder-to-cylinder collision not implemented yet
        // For now, fall back to bounding box intersection as an approximation
        return !(max.x < other.min.x || min.x > other.max.x ||
                 max.y < other.min.y || min.y > other.max.y ||
                 max.z < other.min.z || min.z > other.max.z);
    }
    
    // One cylinder and one box
    const Collider& cylinder = (type == ColliderType::CYLINDER) ? *this : other;
    const Collider& box = (type == ColliderType::CYLINDER) ? other : *this;
    
    // First, check if the bounding boxes overlap (broad phase)
    if (!(cylinder.max.x < box.min.x || cylinder.min.x > box.max.x ||
          cylinder.max.y < box.min.y || cylinder.min.y > box.max.y ||
          cylinder.max.z < box.min.z || cylinder.min.z > box.max.z)) {
        
        // Calculate the closest point on the box to the cylinder axis
        glm::vec3 boxCenter = (box.min + box.max) * 0.5f;
        glm::vec3 boxHalfSize = (box.max - box.min) * 0.5f;
        
        // Horizontal distance check (on XZ plane)
        float dx = std::max(std::abs(boxCenter.x - cylinder.center.x) - boxHalfSize.x, 0.0f);
        float dz = std::max(std::abs(boxCenter.z - cylinder.center.z) - boxHalfSize.z, 0.0f);
        
        // If the horizontal distance is less than the radius, check height intersection
        float horizontalDistSq = dx*dx + dz*dz;
        if (horizontalDistSq <= cylinder.radius * cylinder.radius) {
            // Vertical overlap check
            float cylinderTop = cylinder.center.y + cylinder.height;
            float boxTop = box.max.y;
            float cylinderBottom = cylinder.center.y;
            float boxBottom = box.min.y;
            
            return (cylinderBottom <= boxTop && cylinderTop >= boxBottom);
        }
    }
    
    return false;
}

void Collider::DebugPrint() const {
    std::cout << "Collider:";
    if (type == ColliderType::BOX) {
        std::cout << " [BOX] Min: (" << min.x << ", " << min.y << ", " << min.z << "), "
                  << "Max: (" << max.x << ", " << max.y << ", " << max.z << ")" << std::endl;
    } else {
        std::cout << " [CYLINDER] Base center: (" << center.x << ", " << center.y << ", " << center.z << "), "
                  << "Radius: " << radius << ", Height: " << height << std::endl;
    }
} 