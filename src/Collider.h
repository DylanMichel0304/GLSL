#pragma once
#include <glm/glm.hpp>
#include <cmath>

enum ColliderType {
    BOX,
    CYLINDER
};

class Collider {
public:
    // Default constructor
    Collider() : type(ColliderType::BOX), min(0.0f), max(0.0f) {}
    
    // Constructor for box collider
    Collider(const glm::vec3& minPoint, const glm::vec3& maxPoint);
    
    // Constructor for cylinder collider
    Collider(const glm::vec3& centerPoint, float cylinderRadius, float cylinderHeight);

    // Check if this collider intersects with another collider
    bool Intersects(const Collider& other) const;
    
    // Get the center of the collider
    glm::vec3 Center() const;
    
    // Debug print
    void DebugPrint() const;
    
    // Getter methods
    glm::vec3 getMin() const { return min; }
    glm::vec3 getMax() const { return max; }
    
private:
    // Helper method for cylinder intersection checks
    bool CylinderIntersect(const Collider& other) const;
    
public:
    // Collider properties
    ColliderType type;
    
    // Box properties
    glm::vec3 min;
    glm::vec3 max;

    // Cylinder specific properties
    glm::vec3 center;  // Base center position of the cylinder
    float radius;      // Radius of the cylinder
    float height;      // Height of the cylinder
};
