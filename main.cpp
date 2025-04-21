#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>
#include <vector>

// Include OpenGL headers for macOS
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#endif

#include "shader.h"
#include "camera.h"

// Window settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

// Camera
Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float gameTime = 0.0f; // Global time for animations

// Player state
bool isInAir = false;
float jumpVelocity = 0.0f;
const float GRAVITY = 9.8f;
const float JUMP_FORCE = 5.0f;
const float PLAYER_HEIGHT = 1.0f;

// Terrain settings
const float TERRAIN_SIZE = 200.0f;
const float TERRAIN_HEIGHT = 20.0f;
const int TERRAIN_VERTICES = 40; // Number of vertices per side for the terrain grid

// Decoration settings
const int NUM_TREES = 80;
const int NUM_ROCKS = 50;
const int NUM_FLOWERS = 120;
const int NUM_PALMS = 30;
const int NUM_BEACH_PLANTS = 60;
const int NUM_SHELLS = 40;

// Structure to represent a decoration object
struct DecorationObject {
    glm::vec3 position;
    glm::vec3 scale;
    float rotation;
    int type; // 0 = tree, 1 = rock, 2 = flower, 3 = palm, 4 = beach plant, 5 = shell
};

// Collections of decoration objects
std::vector<DecorationObject> decorations;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Movement keys - increased speed for the larger environment
    const float movementSpeedMultiplier = 5.0f;
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.MovementSpeed = SPEED * movementSpeedMultiplier;
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.MovementSpeed = SPEED * movementSpeedMultiplier;
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.MovementSpeed = SPEED * movementSpeedMultiplier;
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.MovementSpeed = SPEED * movementSpeedMultiplier;
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    
    // Reset movement speed
    camera.MovementSpeed = SPEED;

    // Sprint key
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        camera.MovementSpeed = SPEED * movementSpeedMultiplier * 2.0f;
    }

    // Jump
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !isInAir) {
        isInAir = true;
        jumpVelocity = JUMP_FORCE;
    }
}

// Simple noise function for terrain heightmap
float simpleNoise(float x, float z) {
    return sin(x * 0.1f) * cos(z * 0.1f) * TERRAIN_HEIGHT * 0.5f + 
           sin(x * 0.01f + z * 0.01f) * TERRAIN_HEIGHT * 0.3f + 
           sin(x * 0.2f + z * 0.1f) * cos(x * 0.1f) * TERRAIN_HEIGHT * 0.2f;
}

float getTerrainHeight(float x, float z) {
    // Convert world coordinates to noise function input
    return simpleNoise(x, z);
}

// Create a tree model vertices
std::vector<float> createTreeVertices() {
    std::vector<float> vertices = {
        // Tree trunk (brown)
        // positions          // colors           // texture coords
        -0.2f, 0.0f, -0.2f,   0.5f, 0.3f, 0.0f,   0.0f, 0.0f,
         0.2f, 0.0f, -0.2f,   0.5f, 0.3f, 0.0f,   1.0f, 0.0f,
         0.2f, 2.0f, -0.2f,   0.5f, 0.3f, 0.0f,   1.0f, 1.0f,
        -0.2f, 2.0f, -0.2f,   0.5f, 0.3f, 0.0f,   0.0f, 1.0f,
        
        -0.2f, 0.0f,  0.2f,   0.5f, 0.3f, 0.0f,   0.0f, 0.0f,
         0.2f, 0.0f,  0.2f,   0.5f, 0.3f, 0.0f,   1.0f, 0.0f,
         0.2f, 2.0f,  0.2f,   0.5f, 0.3f, 0.0f,   1.0f, 1.0f,
        -0.2f, 2.0f,  0.2f,   0.5f, 0.3f, 0.0f,   0.0f, 1.0f,
        
        // Tree leaves (green) - pyramids at different heights
        // Bottom pyramid
        -1.0f, 1.0f, -1.0f,   0.0f, 0.6f, 0.0f,   0.0f, 0.0f,
         1.0f, 1.0f, -1.0f,   0.0f, 0.6f, 0.0f,   1.0f, 0.0f,
         1.0f, 1.0f,  1.0f,   0.0f, 0.5f, 0.0f,   1.0f, 1.0f,
        -1.0f, 1.0f,  1.0f,   0.0f, 0.5f, 0.0f,   0.0f, 1.0f,
         0.0f, 2.5f,  0.0f,   0.0f, 0.7f, 0.0f,   0.5f, 0.5f,
        
        // Middle pyramid
        -0.8f, 2.0f, -0.8f,   0.0f, 0.6f, 0.0f,   0.0f, 0.0f,
         0.8f, 2.0f, -0.8f,   0.0f, 0.6f, 0.0f,   1.0f, 0.0f,
         0.8f, 2.0f,  0.8f,   0.0f, 0.5f, 0.0f,   1.0f, 1.0f,
        -0.8f, 2.0f,  0.8f,   0.0f, 0.5f, 0.0f,   0.0f, 1.0f,
         0.0f, 3.5f,  0.0f,   0.0f, 0.7f, 0.0f,   0.5f, 0.5f,
        
        // Top pyramid
        -0.6f, 3.0f, -0.6f,   0.0f, 0.6f, 0.0f,   0.0f, 0.0f,
         0.6f, 3.0f, -0.6f,   0.0f, 0.6f, 0.0f,   1.0f, 0.0f,
         0.6f, 3.0f,  0.6f,   0.0f, 0.5f, 0.0f,   1.0f, 1.0f,
        -0.6f, 3.0f,  0.6f,   0.0f, 0.5f, 0.0f,   0.0f, 1.0f,
         0.0f, 4.5f,  0.0f,   0.0f, 0.7f, 0.0f,   0.5f, 0.5f
    };
    
    return vertices;
}

// Create a rock model vertices
std::vector<float> createRockVertices() {
    std::vector<float> vertices = {
        // Rock (gray) - irregular shape
        // positions          // colors           // texture coords
        -0.5f, 0.0f, -0.5f,   0.5f, 0.5f, 0.5f,   0.0f, 0.0f,
         0.5f, 0.0f, -0.6f,   0.6f, 0.6f, 0.6f,   1.0f, 0.0f,
         0.7f, 0.0f,  0.5f,   0.5f, 0.5f, 0.5f,   1.0f, 1.0f,
        -0.4f, 0.0f,  0.6f,   0.6f, 0.6f, 0.6f,   0.0f, 1.0f,
         0.0f, 0.8f,  0.0f,   0.7f, 0.7f, 0.7f,   0.5f, 0.5f
    };
    
    return vertices;
}

// Create a flower model vertices
std::vector<float> createFlowerVertices() {
    std::vector<float> vertices = {
        // Stem (green)
        // positions          // colors           // texture coords
        -0.02f, 0.0f, 0.0f,   0.0f, 0.5f, 0.0f,   0.0f, 0.0f,
         0.02f, 0.0f, 0.0f,   0.0f, 0.5f, 0.0f,   1.0f, 0.0f,
         0.02f, 0.3f, 0.0f,   0.0f, 0.7f, 0.0f,   1.0f, 1.0f,
        -0.02f, 0.3f, 0.0f,   0.0f, 0.7f, 0.0f,   0.0f, 1.0f,
        
        // Petals (various colors) - implemented as crossed planes
        -0.1f, 0.25f, -0.1f,  0.9f, 0.2f, 0.2f,   0.0f, 0.0f, // Red flower
         0.1f, 0.25f,  0.1f,  0.9f, 0.2f, 0.2f,   1.0f, 1.0f,
        -0.1f, 0.35f, -0.1f,  0.9f, 0.2f, 0.2f,   0.0f, 0.0f,
         0.1f, 0.35f,  0.1f,  0.9f, 0.2f, 0.2f,   1.0f, 1.0f,
         
        -0.1f, 0.25f,  0.1f,  0.9f, 0.5f, 0.1f,   0.0f, 1.0f, // Orange flower
         0.1f, 0.25f, -0.1f,  0.9f, 0.5f, 0.1f,   1.0f, 0.0f,
        -0.1f, 0.35f,  0.1f,  0.9f, 0.5f, 0.1f,   0.0f, 1.0f,
         0.1f, 0.35f, -0.1f,  0.9f, 0.5f, 0.1f,   1.0f, 0.0f,
        
        // Flower center (yellow)
         0.0f, 0.3f,  0.0f,   0.9f, 0.9f, 0.0f,   0.5f, 0.5f,
        -0.05f, 0.3f,  0.0f,  0.9f, 0.9f, 0.0f,   0.0f, 0.5f,
         0.05f, 0.3f,  0.0f,  0.9f, 0.9f, 0.0f,   1.0f, 0.5f,
         0.0f, 0.3f, -0.05f,  0.9f, 0.9f, 0.0f,   0.5f, 0.0f,
         0.0f, 0.3f,  0.05f,  0.9f, 0.9f, 0.0f,   0.5f, 1.0f
    };
    
    return vertices;
}

// Tree indices for triangle drawing
std::vector<unsigned int> createTreeIndices() {
    std::vector<unsigned int> indices = {
        // Trunk - 6 sides, 2 triangles each
        0, 1, 2, 2, 3, 0,     // Front face
        4, 5, 6, 6, 7, 4,     // Back face
        0, 3, 7, 7, 4, 0,     // Left face
        1, 2, 6, 6, 5, 1,     // Right face
        3, 2, 6, 6, 7, 3,     // Top face
        0, 1, 5, 5, 4, 0,     // Bottom face
        
        // Leaves - 3 pyramids, 4 triangles each
        // Bottom pyramid
        8, 9, 14,             // Front face
        9, 10, 14,            // Right face
        10, 11, 14,           // Back face
        11, 8, 14,            // Left face
        
        // Middle pyramid
        15, 16, 19,           // Front face
        16, 17, 19,           // Right face
        17, 18, 19,           // Back face
        18, 15, 19,           // Left face
        
        // Top pyramid
        20, 21, 24,           // Front face
        21, 22, 24,           // Right face
        22, 23, 24,           // Back face
        23, 20, 24            // Left face
    };
    
    return indices;
}

// Rock indices for triangle drawing
std::vector<unsigned int> createRockIndices() {
    std::vector<unsigned int> indices = {
        // Triangles connecting the base vertices to the peak
        0, 1, 4,              // Front face
        1, 2, 4,              // Right face
        2, 3, 4,              // Back face
        3, 0, 4,              // Left face
        // Base quad
        0, 1, 2, 2, 3, 0      // Base face
    };
    
    return indices;
}

// Flower indices for triangle drawing
std::vector<unsigned int> createFlowerIndices() {
    std::vector<unsigned int> indices = {
        // Stem
        0, 1, 2, 2, 3, 0,     // Stem quad
        
        // Petals
        4, 5, 6, 6, 5, 7,     // First crossed plane
        8, 9, 10, 10, 9, 11,  // Second crossed plane
        
        // Center
        12, 13, 14,           // Center triangles
        12, 14, 15,
        12, 15, 16,
        12, 16, 13
    };
    
    return indices;
}

// Create a palm tree model vertices
std::vector<float> createPalmTreeVertices() {
    std::vector<float> vertices = {
        // Palm trunk (brown, slightly curved) - positions, colors, texcoords
        -0.1f, 0.0f, 0.0f,   0.6f, 0.4f, 0.2f,   0.0f, 0.0f,
         0.1f, 0.0f, 0.0f,   0.6f, 0.4f, 0.2f,   1.0f, 0.0f,
         0.2f, 2.0f, 0.0f,   0.5f, 0.3f, 0.1f,   1.0f, 1.0f,
         0.0f, 2.0f, 0.0f,   0.5f, 0.3f, 0.1f,   0.0f, 1.0f,
        
        -0.1f, 0.0f, 0.1f,   0.6f, 0.4f, 0.2f,   0.0f, 0.0f,
         0.1f, 0.0f, 0.1f,   0.6f, 0.4f, 0.2f,   1.0f, 0.0f,
         0.2f, 2.0f, 0.1f,   0.5f, 0.3f, 0.1f,   1.0f, 1.0f,
         0.0f, 2.0f, 0.1f,   0.5f, 0.3f, 0.1f,   0.0f, 1.0f,
        
        // Palm leaves (green, 4 angled fronds) - use triangles
        // Frond 1 - angles up/right
         0.0f, 2.0f, 0.0f,   0.0f, 0.8f, 0.3f,   0.0f, 0.0f,
         0.8f, 2.5f, 0.0f,   0.0f, 0.7f, 0.2f,   0.5f, 0.5f,
         1.2f, 3.2f, 0.2f,   0.0f, 0.8f, 0.3f,   1.0f, 0.0f,
        
        // Frond 2 - angles down/right
         0.0f, 2.0f, 0.0f,   0.0f, 0.8f, 0.3f,   0.0f, 0.0f,
         0.8f, 1.7f, 0.0f,   0.0f, 0.7f, 0.2f,   0.5f, 0.5f,
         1.5f, 1.2f, -0.2f,  0.0f, 0.8f, 0.3f,   1.0f, 0.0f,
        
        // Frond 3 - angles up/left
         0.0f, 2.0f, 0.0f,   0.0f, 0.8f, 0.3f,   0.0f, 0.0f,
        -0.8f, 2.3f, 0.0f,   0.0f, 0.7f, 0.2f,   0.5f, 0.5f,
        -1.3f, 3.0f, 0.2f,   0.0f, 0.8f, 0.3f,   1.0f, 0.0f,
        
        // Frond 4 - angles down/left
         0.0f, 2.0f, 0.0f,   0.0f, 0.8f, 0.3f,   0.0f, 0.0f,
        -0.8f, 1.6f, 0.0f,   0.0f, 0.7f, 0.2f,   0.5f, 0.5f,
        -1.4f, 1.0f, -0.2f,  0.0f, 0.8f, 0.3f,   1.0f, 0.0f,
        
        // Frond 5 - angles forward
         0.0f, 2.0f, 0.0f,   0.0f, 0.8f, 0.3f,   0.0f, 0.0f,
         0.0f, 2.0f, 0.8f,   0.0f, 0.7f, 0.2f,   0.5f, 0.5f,
         0.2f, 2.5f, 1.5f,   0.0f, 0.8f, 0.3f,   1.0f, 0.0f,
        
        // Frond 6 - angles backward
         0.0f, 2.0f, 0.0f,   0.0f, 0.8f, 0.3f,   0.0f, 0.0f,
         0.0f, 2.0f, -0.8f,  0.0f, 0.7f, 0.2f,   0.5f, 0.5f,
        -0.2f, 2.5f, -1.5f,  0.0f, 0.8f, 0.3f,   1.0f, 0.0f
    };
    
    return vertices;
}

// Create a beach plant model vertices (small beach grass/shrub)
std::vector<float> createBeachPlantVertices() {
    std::vector<float> vertices = {
        // Base stem
        -0.05f, 0.0f, 0.0f,  0.7f, 0.7f, 0.6f,   0.0f, 0.0f,
         0.05f, 0.0f, 0.0f,  0.7f, 0.7f, 0.6f,   1.0f, 0.0f,
         0.05f, 0.2f, 0.0f,  0.7f, 0.7f, 0.5f,   1.0f, 1.0f,
        -0.05f, 0.2f, 0.0f,  0.7f, 0.7f, 0.5f,   0.0f, 1.0f,
        
        // Beach grass blades - implementing as crossed planes
        // Blade 1 (tall, center)
        -0.05f, 0.0f, 0.0f,  0.8f, 0.8f, 0.4f,   0.0f, 0.0f,
         0.05f, 0.0f, 0.0f,  0.8f, 0.8f, 0.4f,   1.0f, 0.0f,
         0.1f,  0.8f, 0.0f,  0.7f, 0.8f, 0.3f,   1.0f, 1.0f,
        -0.1f,  0.8f, 0.0f,  0.7f, 0.8f, 0.3f,   0.0f, 1.0f,
        
        // Blade 2 (right side)
         0.05f, 0.0f, -0.05f, 0.8f, 0.7f, 0.3f,   0.0f, 0.0f,
         0.15f, 0.0f,  0.05f, 0.8f, 0.7f, 0.3f,   1.0f, 0.0f,
         0.3f,  0.6f,  0.1f,  0.7f, 0.8f, 0.2f,   1.0f, 1.0f,
         0.2f,  0.6f, -0.1f,  0.7f, 0.8f, 0.2f,   0.0f, 1.0f,
        
        // Blade 3 (left side)
        -0.15f, 0.0f, -0.05f, 0.7f, 0.8f, 0.3f,   0.0f, 0.0f,
        -0.05f, 0.0f,  0.05f, 0.7f, 0.8f, 0.3f,   1.0f, 0.0f,
        -0.2f,  0.7f,  0.1f,  0.6f, 0.8f, 0.2f,   1.0f, 1.0f,
        -0.3f,  0.7f, -0.1f,  0.6f, 0.8f, 0.2f,   0.0f, 1.0f,
        
        // Blade 4 (back)
        -0.05f, 0.0f, -0.15f, 0.7f, 0.7f, 0.3f,   0.0f, 0.0f,
         0.05f, 0.0f, -0.05f, 0.7f, 0.7f, 0.3f,   1.0f, 0.0f,
         0.1f,  0.5f, -0.3f,  0.6f, 0.7f, 0.2f,   1.0f, 1.0f,
        -0.1f,  0.5f, -0.4f,  0.6f, 0.7f, 0.2f,   0.0f, 1.0f,
        
        // Blade 5 (front)
        -0.05f, 0.0f,  0.05f, 0.8f, 0.8f, 0.4f,   0.0f, 0.0f,
         0.05f, 0.0f,  0.15f, 0.8f, 0.8f, 0.4f,   1.0f, 0.0f,
         0.1f,  0.6f,  0.4f,  0.7f, 0.9f, 0.3f,   1.0f, 1.0f,
        -0.1f,  0.6f,  0.3f,  0.7f, 0.9f, 0.3f,   0.0f, 1.0f
    };
    
    return vertices;
}

// Create a shell model vertices
std::vector<float> createShellVertices() {
    std::vector<float> vertices = {
        // Shell base (circular, pale color)
        // Center point
         0.0f, 0.05f, 0.0f,   0.9f, 0.85f, 0.75f,  0.5f, 0.5f,
        
        // Points around the circle - 8 points for a simple shell
        -0.1f, 0.0f, -0.1f,   0.95f, 0.9f, 0.8f,   0.0f, 0.0f,
         0.1f, 0.0f, -0.1f,   0.9f, 0.85f, 0.75f,  1.0f, 0.0f,
         0.15f, 0.0f, 0.0f,   0.85f, 0.8f, 0.7f,   1.0f, 0.5f,
         0.1f, 0.0f, 0.1f,    0.9f, 0.85f, 0.75f,  1.0f, 1.0f,
        -0.1f, 0.0f, 0.1f,    0.95f, 0.9f, 0.8f,   0.0f, 1.0f,
        -0.15f, 0.0f, 0.0f,   0.85f, 0.8f, 0.7f,   0.0f, 0.5f,
        
        // Spiral top part of shell
         0.0f, 0.0f, 0.0f,    0.9f, 0.85f, 0.75f,  0.5f, 0.5f,
         0.05f, 0.1f, 0.05f,  0.85f, 0.8f, 0.7f,   0.6f, 0.6f,
         0.1f, 0.15f, 0.0f,   0.8f, 0.75f, 0.65f,  0.7f, 0.5f,
         0.05f, 0.2f, -0.05f, 0.75f, 0.7f, 0.6f,   0.6f, 0.4f
    };
    
    return vertices;
}

// Palm tree indices for triangle drawing
std::vector<unsigned int> createPalmTreeIndices() {
    std::vector<unsigned int> indices = {
        // Trunk - 6 sides, 2 triangles each
        0, 1, 2, 2, 3, 0,     // Front face
        4, 5, 6, 6, 7, 4,     // Back face
        0, 3, 7, 7, 4, 0,     // Left face
        1, 2, 6, 6, 5, 1,     // Right face
        3, 2, 6, 6, 7, 3,     // Top face
        0, 1, 5, 5, 4, 0,     // Bottom face
        
        // Leaves - 6 fronds, each a triangle
        8, 9, 10,             // Frond 1
        11, 12, 13,           // Frond 2
        14, 15, 16,           // Frond 3
        17, 18, 19,           // Frond 4
        20, 21, 22,           // Frond 5
        23, 24, 25            // Frond 6
    };
    
    return indices;
}

// Beach plant indices for triangle drawing
std::vector<unsigned int> createBeachPlantIndices() {
    std::vector<unsigned int> indices = {
        // Base stem
        0, 1, 2, 2, 3, 0,
        
        // Grass blades - each is a quad (2 triangles)
        4, 5, 6, 6, 7, 4,     // Blade 1
        8, 9, 10, 10, 11, 8,  // Blade 2
        12, 13, 14, 14, 15, 12, // Blade 3
        16, 17, 18, 18, 19, 16, // Blade 4
        20, 21, 22, 22, 23, 20  // Blade 5
    };
    
    return indices;
}

// Shell indices for triangle drawing
std::vector<unsigned int> createShellIndices() {
    std::vector<unsigned int> indices = {
        // Base of shell - triangles from center to perimeter
        0, 1, 2,
        0, 2, 3,
        0, 3, 4,
        0, 4, 5,
        0, 5, 1,
        
        // Spiral top part
        7, 8, 9,
        7, 9, 10
    };
    
    return indices;
}

// Generate random decorations across the terrain
void generateDecorations() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> posDistrib(-TERRAIN_SIZE/2.0f + 10.0f, TERRAIN_SIZE/2.0f - 10.0f);
    std::uniform_real_distribution<float> scaleDistrib(0.7f, 1.5f);
    std::uniform_real_distribution<float> rotDistrib(0.0f, 360.0f);
    
    // Generate regular trees (mainly in higher areas)
    for (int i = 0; i < NUM_TREES; i++) {
        float x = posDistrib(gen);
        float z = posDistrib(gen);
        float y = getTerrainHeight(x, z);
        
        // Don't place trees on steep slopes, in water, or on the beach
        if (abs(y) > 12.0f || y < 0.5f) continue;
        
        DecorationObject tree;
        tree.position = glm::vec3(x, y, z);
        tree.scale = glm::vec3(scaleDistrib(gen));
        tree.rotation = rotDistrib(gen);
        tree.type = 0; // Tree
        
        decorations.push_back(tree);
    }
    
    // Generate palm trees (only near water/beach areas)
    for (int i = 0; i < NUM_PALMS; i++) {
        float x = posDistrib(gen);
        float z = posDistrib(gen);
        float y = getTerrainHeight(x, z);
        
        // Only place palm trees near the shoreline on beach areas
        if (y < -0.5f || y > 2.0f) continue;
        
        DecorationObject palm;
        palm.position = glm::vec3(x, y, z);
        palm.scale = glm::vec3(scaleDistrib(gen) * 0.8f); // Slightly smaller
        palm.rotation = rotDistrib(gen);
        palm.type = 3; // Palm tree
        
        decorations.push_back(palm);
    }
    
    // Generate rocks
    for (int i = 0; i < NUM_ROCKS; i++) {
        float x = posDistrib(gen);
        float z = posDistrib(gen);
        float y = getTerrainHeight(x, z);
        
        DecorationObject rock;
        rock.position = glm::vec3(x, y, z);
        rock.scale = glm::vec3(scaleDistrib(gen) * 0.8f);
        rock.rotation = rotDistrib(gen);
        rock.type = 1; // Rock
        
        decorations.push_back(rock);
    }
    
    // Generate flowers (only in grassy areas)
    for (int i = 0; i < NUM_FLOWERS; i++) {
        float x = posDistrib(gen);
        float z = posDistrib(gen);
        float y = getTerrainHeight(x, z);
        
        // Only place flowers on relatively flat, grassy areas
        if (y < 0.5f || y > 8.0f) continue;
        
        DecorationObject flower;
        flower.position = glm::vec3(x, y, z);
        flower.scale = glm::vec3(scaleDistrib(gen) * 0.5f);
        flower.rotation = rotDistrib(gen);
        flower.type = 2; // Flower
        
        decorations.push_back(flower);
    }
    
    // Generate beach plants (only on beach areas)
    for (int i = 0; i < NUM_BEACH_PLANTS; i++) {
        float x = posDistrib(gen);
        float z = posDistrib(gen);
        float y = getTerrainHeight(x, z);
        
        // Only place beach plants on the beach/shore
        if (y < -0.5f || y > 1.5f) continue;
        
        DecorationObject beachPlant;
        beachPlant.position = glm::vec3(x, y, z);
        beachPlant.scale = glm::vec3(scaleDistrib(gen) * 0.7f);
        beachPlant.rotation = rotDistrib(gen);
        beachPlant.type = 4; // Beach plant
        
        decorations.push_back(beachPlant);
    }
    
    // Generate shells (only on beaches and shallow water)
    for (int i = 0; i < NUM_SHELLS; i++) {
        float x = posDistrib(gen);
        float z = posDistrib(gen);
        float y = getTerrainHeight(x, z);
        
        // Only place shells on beach or in very shallow water
        if (y < -1.5f || y > 0.5f) continue;
        
        DecorationObject shell;
        shell.position = glm::vec3(x, y + 0.01f, z); // Slightly above ground
        shell.scale = glm::vec3(scaleDistrib(gen) * 0.4f); // Shells are small
        shell.rotation = rotDistrib(gen);
        shell.type = 5; // Shell
        
        decorations.push_back(shell);
    }
}

void updatePlayerPosition() {
    // Get the current terrain height at player's position
    float terrainHeightAtPlayer = getTerrainHeight(camera.Position.x, camera.Position.z);
    
    // Apply gravity and handle jumping
    if (isInAir) {
        // Update player position based on jump velocity
        camera.Position.y += jumpVelocity * deltaTime;
        
        // Apply gravity to jump velocity
        jumpVelocity -= GRAVITY * deltaTime;
        
        // Check if player has landed
        if (camera.Position.y <= terrainHeightAtPlayer + PLAYER_HEIGHT) {
            camera.Position.y = terrainHeightAtPlayer + PLAYER_HEIGHT;
            isInAir = false;
            jumpVelocity = 0.0f;
        }
    } else {
        // Ensure player stays at the correct height above terrain when not jumping
        camera.Position.y = terrainHeightAtPlayer + PLAYER_HEIGHT;
    }
    
    // Boundary check - prevent player from falling off the terrain
    if (camera.Position.x < -TERRAIN_SIZE * 0.5f) camera.Position.x = -TERRAIN_SIZE * 0.5f;
    if (camera.Position.x > TERRAIN_SIZE * 0.5f) camera.Position.x = TERRAIN_SIZE * 0.5f;
    if (camera.Position.z < -TERRAIN_SIZE * 0.5f) camera.Position.z = -TERRAIN_SIZE * 0.5f;
    if (camera.Position.z > TERRAIN_SIZE * 0.5f) camera.Position.z = TERRAIN_SIZE * 0.5f;
}

int main() {
    std::cout << "Starting 3D GLSL Game..." << std::endl;
    
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create a window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D GLSL Outdoor Environment", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Capture mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Load OpenGL function pointers
#ifdef __APPLE__
    // On macOS, the OpenGL functions are already loaded
#else
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }
#endif

    // Enable depth testing and backface culling
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    std::cout << "Building and compiling shaders..." << std::endl;
    
    // Build and compile shaders
    Shader shader("vertex.glsl", "fragment.glsl");

    // Generate terrain mesh (grid)
    std::vector<float> terrainVertices;
    std::vector<unsigned int> terrainIndices;
    
    // Calculate step size
    float step = TERRAIN_SIZE / static_cast<float>(TERRAIN_VERTICES - 1);
    
    // Generate vertices for the terrain grid
    for (int z = 0; z < TERRAIN_VERTICES; z++) {
        for (int x = 0; x < TERRAIN_VERTICES; x++) {
            float xPos = x * step - TERRAIN_SIZE / 2.0f;
            float zPos = z * step - TERRAIN_SIZE / 2.0f;
            float yPos = getTerrainHeight(xPos, zPos);
            
            // Position
            terrainVertices.push_back(xPos);
            terrainVertices.push_back(yPos);
            terrainVertices.push_back(zPos);
            
            // Color - more natural terrain coloring
            float normalizedHeight = (yPos + TERRAIN_HEIGHT) / (2.0f * TERRAIN_HEIGHT);
            
            // Define natural color ranges
            float r, g, b;
            
            if (yPos < -5.0f) {
                // Deep water (dark blue)
                r = 0.0f;
                g = 0.1f;
                b = 0.4f;
            } 
            else if (yPos < -2.0f) {
                // Shallow water (lighter blue)
                r = 0.0f;
                g = 0.2f;
                b = 0.5f;
            }
            else if (yPos < 0.5f) {
                // Beach/shore (sandy color)
                r = 0.76f;
                g = 0.7f;
                b = 0.5f;
            }
            else if (yPos < 8.0f) {
                // Grass/plains (green)
                r = 0.2f;
                g = 0.5f + normalizedHeight * 0.2f;
                b = 0.1f;
            }
            else if (yPos < 12.0f) {
                // Forest/hill transition (darker green to brown)
                float t = (yPos - 8.0f) / 4.0f; // 0 to 1 transition factor
                r = 0.2f + t * 0.3f;
                g = 0.5f - t * 0.2f;
                b = 0.1f;
            }
            else {
                // Mountain (gray/white for peaks)
                float t = glm::clamp((yPos - 12.0f) / 8.0f, 0.0f, 1.0f);
                r = 0.5f + t * 0.4f;
                g = 0.5f + t * 0.4f;
                b = 0.5f + t * 0.4f;
            }
            
            terrainVertices.push_back(r);
            terrainVertices.push_back(g);
            terrainVertices.push_back(b);
            
            // Texture coordinates
            terrainVertices.push_back(static_cast<float>(x) / (TERRAIN_VERTICES - 1));
            terrainVertices.push_back(static_cast<float>(z) / (TERRAIN_VERTICES - 1));
        }
    }
    
    // Generate indices for the terrain grid (triangles)
    for (int z = 0; z < TERRAIN_VERTICES - 1; z++) {
        for (int x = 0; x < TERRAIN_VERTICES - 1; x++) {
            // Calculate indices for the two triangles forming a quad
            unsigned int topLeft = z * TERRAIN_VERTICES + x;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = (z + 1) * TERRAIN_VERTICES + x;
            unsigned int bottomRight = bottomLeft + 1;
            
            // Triangle 1
            terrainIndices.push_back(topLeft);
            terrainIndices.push_back(bottomLeft);
            terrainIndices.push_back(topRight);
            
            // Triangle 2
            terrainIndices.push_back(topRight);
            terrainIndices.push_back(bottomLeft);
            terrainIndices.push_back(bottomRight);
        }
    }
    
    std::cout << "Terrain generated with " << terrainVertices.size() / 8 << " vertices" << std::endl;
    
    // Create decoration meshes
    std::vector<float> treeVertices = createTreeVertices();
    std::vector<unsigned int> treeIndices = createTreeIndices();
    
    std::vector<float> rockVertices = createRockVertices();
    std::vector<unsigned int> rockIndices = createRockIndices();
    
    std::vector<float> flowerVertices = createFlowerVertices();
    std::vector<unsigned int> flowerIndices = createFlowerIndices();
    
    // Island-specific decorations
    std::vector<float> palmVertices = createPalmTreeVertices();
    std::vector<unsigned int> palmIndices = createPalmTreeIndices();
    
    std::vector<float> beachPlantVertices = createBeachPlantVertices();
    std::vector<unsigned int> beachPlantIndices = createBeachPlantIndices();
    
    std::vector<float> shellVertices = createShellVertices();
    std::vector<unsigned int> shellIndices = createShellIndices();
    
    // Generate random decorations
    generateDecorations();
    std::cout << "Generated " << decorations.size() << " decoration objects" << std::endl;
    
    // Set up buffers for the terrain
    unsigned int terrainVBO, terrainVAO, terrainEBO;
    glGenVertexArrays(1, &terrainVAO);
    glGenBuffers(1, &terrainVBO);
    glGenBuffers(1, &terrainEBO);

    glBindVertexArray(terrainVAO);

    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, terrainVertices.size() * sizeof(float), terrainVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, terrainIndices.size() * sizeof(unsigned int), terrainIndices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    // Set up buffers for tree objects
    unsigned int treeVBO, treeVAO, treeEBO;
    glGenVertexArrays(1, &treeVAO);
    glGenBuffers(1, &treeVBO);
    glGenBuffers(1, &treeEBO);

    glBindVertexArray(treeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, treeVBO);
    glBufferData(GL_ARRAY_BUFFER, treeVertices.size() * sizeof(float), treeVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, treeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, treeIndices.size() * sizeof(unsigned int), treeIndices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    // Set up buffers for rock objects
    unsigned int rockVBO, rockVAO, rockEBO;
    glGenVertexArrays(1, &rockVAO);
    glGenBuffers(1, &rockVBO);
    glGenBuffers(1, &rockEBO);

    glBindVertexArray(rockVAO);

    glBindBuffer(GL_ARRAY_BUFFER, rockVBO);
    glBufferData(GL_ARRAY_BUFFER, rockVertices.size() * sizeof(float), rockVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rockEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, rockIndices.size() * sizeof(unsigned int), rockIndices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    // Set up buffers for flower objects
    unsigned int flowerVBO, flowerVAO, flowerEBO;
    glGenVertexArrays(1, &flowerVAO);
    glGenBuffers(1, &flowerVBO);
    glGenBuffers(1, &flowerEBO);

    glBindVertexArray(flowerVAO);

    glBindBuffer(GL_ARRAY_BUFFER, flowerVBO);
    glBufferData(GL_ARRAY_BUFFER, flowerVertices.size() * sizeof(float), flowerVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, flowerEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, flowerIndices.size() * sizeof(unsigned int), flowerIndices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Set up buffers for palm tree objects
    unsigned int palmVBO, palmVAO, palmEBO;
    glGenVertexArrays(1, &palmVAO);
    glGenBuffers(1, &palmVBO);
    glGenBuffers(1, &palmEBO);

    glBindVertexArray(palmVAO);

    glBindBuffer(GL_ARRAY_BUFFER, palmVBO);
    glBufferData(GL_ARRAY_BUFFER, palmVertices.size() * sizeof(float), palmVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, palmEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, palmIndices.size() * sizeof(unsigned int), palmIndices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    // Set up buffers for beach plant objects
    unsigned int beachPlantVBO, beachPlantVAO, beachPlantEBO;
    glGenVertexArrays(1, &beachPlantVAO);
    glGenBuffers(1, &beachPlantVBO);
    glGenBuffers(1, &beachPlantEBO);

    glBindVertexArray(beachPlantVAO);

    glBindBuffer(GL_ARRAY_BUFFER, beachPlantVBO);
    glBufferData(GL_ARRAY_BUFFER, beachPlantVertices.size() * sizeof(float), beachPlantVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, beachPlantEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, beachPlantIndices.size() * sizeof(unsigned int), beachPlantIndices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    // Set up buffers for shell objects
    unsigned int shellVBO, shellVAO, shellEBO;
    glGenVertexArrays(1, &shellVAO);
    glGenBuffers(1, &shellVBO);
    glGenBuffers(1, &shellEBO);

    glBindVertexArray(shellVAO);

    glBindBuffer(GL_ARRAY_BUFFER, shellVBO);
    glBufferData(GL_ARRAY_BUFFER, shellVertices.size() * sizeof(float), shellVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shellEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, shellIndices.size() * sizeof(unsigned int), shellIndices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Set initial player height
    float initialTerrainHeight = getTerrainHeight(camera.Position.x, camera.Position.z);
    camera.Position.y = initialTerrainHeight + PLAYER_HEIGHT;
    
    std::cout << "Starting game loop..." << std::endl;

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        // Calculate delta time
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        gameTime += deltaTime; // Update the global time

        // Process input
        processInput(window);
        
        // Update player position (jumping/gravity)
        updatePlayerPosition();

        // Render
        glClearColor(0.5f, 0.8f, 0.9f, 1.0f);  // Sky blue background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Activate shader
        shader.use();
        
        // Pass the time to the shader for water animation
        shader.setFloat("time", gameTime);

        // Create transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        
        // Render terrain
        glm::mat4 model = glm::mat4(1.0f);
        shader.setMat4("model", model);
        
        glBindVertexArray(terrainVAO);
        glDrawElements(GL_TRIANGLES, terrainIndices.size(), GL_UNSIGNED_INT, 0);
        
        // Render decorations
        for (const auto& decoration : decorations) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, decoration.position);
            model = glm::rotate(model, glm::radians(decoration.rotation), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, decoration.scale);
            shader.setMat4("model", model);
            
            switch (decoration.type) {
                case 0: // Tree
                    glBindVertexArray(treeVAO);
                    glDrawElements(GL_TRIANGLES, treeIndices.size(), GL_UNSIGNED_INT, 0);
                    break;
                case 1: // Rock
                    glBindVertexArray(rockVAO);
                    glDrawElements(GL_TRIANGLES, rockIndices.size(), GL_UNSIGNED_INT, 0);
                    break;
                case 2: // Flower
                    glBindVertexArray(flowerVAO);
                    glDrawElements(GL_TRIANGLES, flowerIndices.size(), GL_UNSIGNED_INT, 0);
                    break;
                case 3: // Palm tree
                    glBindVertexArray(palmVAO);
                    glDrawElements(GL_TRIANGLES, palmIndices.size(), GL_UNSIGNED_INT, 0);
                    break;
                case 4: // Beach plant
                    glBindVertexArray(beachPlantVAO);
                    glDrawElements(GL_TRIANGLES, beachPlantIndices.size(), GL_UNSIGNED_INT, 0);
                    break;
                case 5: // Shell
                    glBindVertexArray(shellVAO);
                    glDrawElements(GL_TRIANGLES, shellIndices.size(), GL_UNSIGNED_INT, 0);
                    break;
            }
        }

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    glDeleteVertexArrays(1, &terrainVAO);
    glDeleteBuffers(1, &terrainVBO);
    glDeleteBuffers(1, &terrainEBO);
    
    glDeleteVertexArrays(1, &treeVAO);
    glDeleteBuffers(1, &treeVBO);
    glDeleteBuffers(1, &treeEBO);
    
    glDeleteVertexArrays(1, &rockVAO);
    glDeleteBuffers(1, &rockVBO);
    glDeleteBuffers(1, &rockEBO);
    
    glDeleteVertexArrays(1, &flowerVAO);
    glDeleteBuffers(1, &flowerVBO);
    glDeleteBuffers(1, &flowerEBO);

    glDeleteVertexArrays(1, &palmVAO);
    glDeleteBuffers(1, &palmVBO);
    glDeleteBuffers(1, &palmEBO);
    
    glDeleteVertexArrays(1, &beachPlantVAO);
    glDeleteBuffers(1, &beachPlantVBO);
    glDeleteBuffers(1, &beachPlantEBO);
    
    glDeleteVertexArrays(1, &shellVAO);
    glDeleteBuffers(1, &shellVBO);
    glDeleteBuffers(1, &shellEBO);

    glfwTerminate();
    return 0;
} 