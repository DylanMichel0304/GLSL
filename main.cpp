#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    
    // Calculate step size
    float step = TERRAIN_SIZE / static_cast<float>(TERRAIN_VERTICES - 1);
    
    // Generate vertices for the terrain grid
    for (int z = 0; z < TERRAIN_VERTICES; z++) {
        for (int x = 0; x < TERRAIN_VERTICES; x++) {
            float xPos = x * step - TERRAIN_SIZE / 2.0f;
            float zPos = z * step - TERRAIN_SIZE / 2.0f;
            float yPos = getTerrainHeight(xPos, zPos);
            
            // Position
            vertices.push_back(xPos);
            vertices.push_back(yPos);
            vertices.push_back(zPos);
            
            // Color - varies based on height
            float normalizedHeight = (yPos + TERRAIN_HEIGHT) / (2.0f * TERRAIN_HEIGHT);
            
            // Grass (green) for flat/medium areas
            float g = glm::clamp(0.4f + normalizedHeight * 0.4f, 0.0f, 0.8f);
            // Brown for peaks
            float r = glm::clamp(normalizedHeight * 0.8f, 0.1f, 0.7f);
            // Blue-ish for valleys
            float b = glm::clamp(0.1f + (1.0f - normalizedHeight) * 0.3f, 0.0f, 0.4f);
            
            vertices.push_back(r);
            vertices.push_back(g);
            vertices.push_back(b);
            
            // Texture coordinates
            vertices.push_back(static_cast<float>(x) / (TERRAIN_VERTICES - 1));
            vertices.push_back(static_cast<float>(z) / (TERRAIN_VERTICES - 1));
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
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);
            
            // Triangle 2
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
    
    std::cout << "Terrain generated with " << vertices.size() / 8 << " vertices" << std::endl;
    
    // Set up buffers
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

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

        // Process input
        processInput(window);
        
        // Update player position (jumping/gravity)
        updatePlayerPosition();

        // Render
        glClearColor(0.5f, 0.8f, 0.9f, 1.0f);  // Sky blue background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Activate shader
        shader.use();

        // Create transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        
        // Model matrix
        glm::mat4 model = glm::mat4(1.0f);
        shader.setMat4("model", model);

        // Render terrain
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
} 