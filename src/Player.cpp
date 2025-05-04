#include "Player.h"

Player::Player(int width, int height, glm::vec3 startPos)
    : camera(width, height, startPos), speed(30.0f) // vitesse augmentée
{
    // Définir un collider autour du joueur (taille réduite)
    glm::vec3 halfSize(0.15f, 0.5f, 0.15f); // box plus petite
    collider.min = startPos - halfSize;
    collider.max = startPos + halfSize;
}

void Player::Update(GLFWwindow* window, const std::vector<Collider>& worldColliders, float deltaTime)
{
    glm::vec3 moveDir(0.0f);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        moveDir += camera.Orientation;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        moveDir -= camera.Orientation;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        moveDir -= glm::normalize(glm::cross(camera.Orientation, camera.Up));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        moveDir += glm::normalize(glm::cross(camera.Orientation, camera.Up));
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        moveDir += -camera.Up; // Descendre
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        moveDir += camera.Up; // Monter

    if (glm::length(moveDir) > 0.0f)
        moveDir = glm::normalize(moveDir);

    glm::vec3 newPos = camera.Position;
    glm::vec3 delta = moveDir * speed * deltaTime;

    // Test collision sur chaque axe séparément
    for (int axis = 0; axis < 3; ++axis) {
        glm::vec3 testPos = newPos;
        testPos[axis] += delta[axis];
        if (!CheckCollision(testPos, worldColliders)) {
            newPos[axis] += delta[axis];
        }
    }

    camera.Position = newPos;

    // Mettre à jour le collider
    glm::vec3 halfSize = (collider.max - collider.min) * 0.5f;
    collider.min = camera.Position - halfSize;
    collider.max = camera.Position + halfSize;

    camera.Inputs(window);
    camera.updateMatrix(45.0f, 0.1f, 100.0f);
}

bool Player::CheckCollision(glm::vec3 newPosition, const std::vector<Collider>& worldColliders)
{
    glm::vec3 halfSize = (collider.max - collider.min) * 0.5f;
    Collider tempCollider(newPosition - halfSize, newPosition + halfSize);

    for (const Collider& c : worldColliders)
    {
        if (tempCollider.Intersects(c))
        {
            std::cout << "Collision detected with box: min(" 
                      << c.min.x << "," << c.min.y << "," << c.min.z << ") max("
                      << c.max.x << "," << c.max.y << "," << c.max.z << ")" << std::endl;
            return true;
        }
    }
    return false;
}
