#include"src/Mesh.h"
#include"src/model.h"
#include "src/Player.h"
#include "src/Collider.h"
#include "src/Light.h"
#include<iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include "import/stb/stb_image.h"


const unsigned int width = 800;
const unsigned int height = 800;

GLFWwindow* init_window() {
    // Initialize GLFW
    glfwInit();

    // Tell GLFW what version of OpenGL we are using 
    // In this case we are using OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Tell GLFW we are using the CORE profile
    // So that means we only have the modern functions
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
    GLFWwindow* window = glfwCreateWindow(width, height, "YoutubeOpenGL", NULL, NULL);
    // Error check if the window fails to create
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    // Introduce the window into the current context
    glfwMakeContextCurrent(window);
    // Load GLAD so it configures OpenGL
    gladLoadGL();
    // Specify the viewport of OpenGL in the Window
    // In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
    glViewport(0, 0, width, height);
    return window;
}

int main()
{
    GLFWwindow* window = init_window();
    std::string texPath = "assets/textures/"; // Path to the textures
    
    // Initialize player at a reasonable height above ground
    Player player(width, height, glm::vec3(0.0f, 1.7f, 0.0f));
    player.speed = 5.0f;  // More realistic movement speed for first-person

    // Texture data
    Texture textures[]
    {
        Texture((texPath + "herbe.png").c_str(), "diffuse", 0),
        //Texture((texPath + "planksSpec.png").c_str(), "specular", 1, GL_RED, GL_UNSIGNED_BYTE)
    };

    // Load the textures
    Shader shaderProgram("shader/default.vert", "shader/default.frag");
    Shader lightShader("shader/light.vert", "shader/light.frag");

    // Load the models

    // Model matrix 

    // Add colliders to world colliders
    std::vector<Collider> worldColliders;

    // Enables the Depth Buffer
    glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);

    std::vector<Light> sceneLights;

    // Directional light (sun)
    sceneLights.emplace_back(
        0, // type
        glm::vec3(0.0f), // position (inutile pour directionnelle)
        glm::vec3(-0.2f, -1.0f, -0.3f), // direction
        glm::vec4(1.0f, 1.0f, 0.9f, 1.0f), // color
        nullptr
    );

    // Spot light (lamp) avec mesh associé
    sceneLights.emplace_back(
        2, // type
        glm::vec3(0.0f, 50.0f, 0.0f), // position
        glm::vec3(0.0f, -1.0f, 0.0f), // direction
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), // color
        nullptr // mesh associé
    );


    // Main while loop
    while (!glfwWindowShouldClose(window))
    {
        // Specify the color of the background
        glClearColor(0.3f, 0.1f, 0.1f, 1.0f); // Dark red/brown to contrast with the terrain
        // Clean the back buffer and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Update du joueur
        float deltaTime = glfwGetTime();
        player.Update(window, worldColliders, deltaTime);



        // Swap the back buffer with the front buffer
        glfwSwapBuffers(window);
        // Take care of all GLFW events
        glfwPollEvents();
    }

    // Delete all the objects we've created
    shaderProgram.Delete();
    lightShader.Delete();




    // Delete window before ending the program
    glfwDestroyWindow(window);
    // Terminate GLFW before ending the program
    glfwTerminate();
    return 0;
}