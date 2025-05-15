#include "Player.h"

Player::Player(int width, int height, glm::vec3 startPos)
    : camera(width, height, startPos), speed(30.0f),
      gravity(20.0f), jumpForce(10.0f), verticalVelocity(0.0f),
      isGrounded(false), groundLevel(0.0f)
{
    // Set player at ground level initially
    camera.Position.y = groundLevel + 1.7f; // Eye level height
    
    // Define a collider around the player (smaller box)
    glm::vec3 halfSize(0.3f, 0.85f, 0.3f); // Player capsule dimensions
    collider.min = camera.Position - halfSize;
    collider.max = camera.Position + halfSize;
}

void Player::Update(GLFWwindow* window, const std::vector<Collider>& worldColliders, float deltaTime)
{
    // Apply gravity and check if grounded
    ApplyGravity(deltaTime);
    
    // Handle jumping
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && isGrounded)
    {
        verticalVelocity = jumpForce;
        isGrounded = false;
    }
    
    glm::vec3 moveDir(0.0f);

    // Horizontal movement (XZ plane)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        glm::vec3 forward = camera.Orientation;
        forward.y = 0.0f; // Project onto XZ plane
        if (glm::length(forward) > 0.0f)
            moveDir += glm::normalize(forward);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        glm::vec3 backward = -camera.Orientation;
        backward.y = 0.0f; // Project onto XZ plane
        if (glm::length(backward) > 0.0f)
            moveDir += glm::normalize(backward);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        glm::vec3 left = -glm::normalize(glm::cross(camera.Orientation, camera.Up));
        left.y = 0.0f; // Project onto XZ plane
        if (glm::length(left) > 0.0f)
            moveDir += glm::normalize(left);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        glm::vec3 right = glm::normalize(glm::cross(camera.Orientation, camera.Up));
        right.y = 0.0f; // Project onto XZ plane
        if (glm::length(right) > 0.0f)
            moveDir += glm::normalize(right);
    }

    if (glm::length(moveDir) > 0.0f)
        moveDir = glm::normalize(moveDir);

    glm::vec3 newPos = camera.Position;
    
    // Apply horizontal movement
    glm::vec3 horizontalDelta = moveDir * speed * deltaTime;

    // Apply movement along XZ plane
    for (int axis = 0; axis < 3; ++axis) {
        // Skip Y axis as it's handled by gravity/jumping
        if (axis == 1) continue;
        
        glm::vec3 testPos = newPos;
        testPos[axis] += horizontalDelta[axis];
        if (!CheckCollision(testPos, worldColliders)) {
            newPos[axis] += horizontalDelta[axis];
        }
    }
    
    // Apply vertical movement from gravity/jumping
    glm::vec3 testPos = newPos;
    testPos.y += verticalVelocity * deltaTime;
    if (!CheckCollision(testPos, worldColliders)) {
        newPos.y += verticalVelocity * deltaTime;
    } else {
        // Hit something - if moving down, we've hit ground
        if (verticalVelocity < 0) {
            isGrounded = true;
        }
        verticalVelocity = 0;
    }

    camera.Position = newPos;

    // Update the collider
    glm::vec3 halfSize = (collider.max - collider.min) * 0.5f;
    collider.min = camera.Position - halfSize;
    collider.max = camera.Position + halfSize;

    camera.Inputs(window);
    camera.updateMatrix(45.0f, 0.1f, 100.0f);
}

void Player::ApplyGravity(float deltaTime)
{
    // Apply gravity if not on ground
    if (!isGrounded) {
        verticalVelocity -= gravity * deltaTime;
    }
    
    // Check if we're on the ground
    CheckGrounded();
}

void Player::CheckGrounded()
{
    // If we're very close to ground level, snap to it and set grounded
    if (camera.Position.y - 1.7f <= groundLevel + 0.1f && verticalVelocity <= 0) {
        camera.Position.y = groundLevel + 1.7f;
        verticalVelocity = 0.0f;
        isGrounded = true;
    }
}

bool Player::CheckCollision(glm::vec3 newPosition, const std::vector<Collider>& worldColliders)
{
    glm::vec3 halfSize = (collider.max - collider.min) * 0.5f;
    Collider tempCollider(newPosition - halfSize, newPosition + halfSize);

    for (const Collider& c : worldColliders)
    {
        if (tempCollider.Intersects(c))
        {
            return true;
        }
    }
    return false;
}
