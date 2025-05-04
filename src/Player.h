#ifndef PLAYER_CLASS_H
#define PLAYER_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Camera.h"
#include "Collider.h"
#include <vector>

class Player
{
public:
    Camera camera;
    Collider collider;
    float speed;

    // Physics properties
    float gravity;
    float jumpForce;
    float verticalVelocity;
    bool isGrounded;
    float groundLevel; // Y-coordinate of the ground

    Player(int width, int height, glm::vec3 startPos);

    void Update(GLFWwindow* window, const std::vector<Collider>& worldColliders, float deltaTime);

private:
    bool CheckCollision(glm::vec3 newPosition, const std::vector<Collider>& worldColliders);
    void ApplyGravity(float deltaTime);
    void CheckGrounded();
};

#endif
