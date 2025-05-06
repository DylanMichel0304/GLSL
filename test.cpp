#include"src/Mesh.h"
#include"src/model.h"
#include<iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "src/Player.h"
#include "src/Collider.h"
#include "src/Light.h"
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <cmath>

// Define this before including stb_image.h
#define STB_IMAGE_IMPLEMENTATION
#include "import/stb/stb_image.h"


const unsigned int width = 800;
const unsigned int height = 800;


int main()
{
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
    GLFWwindow* window = glfwCreateWindow(width, height, "3D_game", NULL, NULL);
    // Error check if the window fails to create
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // Introduce the window into the current context
    glfwMakeContextCurrent(window);

    // Load GLAD so it configures OpenGL
    gladLoadGL();
    // Specify the viewport of OpenGL in the Window
    // In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
    glViewport(0, 0, width, height);

    std::string texPath = "assets/textures/"; // Path to the textures

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
    Model terrainModel("assets/objects/plane.obj");
    Model treeModel("assets/objects/Tree 02/Tree.obj");
    Model farmhouseModel("assets/textures/newhouse/farmhouse_obj.obj");
    Model lightSphereModel("assets/objects/sphere.obj");
    Model wallModel("assets/objects/plane.obj");
    Model lampModel("assets/objects/LAMP/rv_lamp_post_4.obj", true); // Load lamp with collider

    // Set different texture tiling for each model
    terrainModel.SetTextureTiling(150.0f);  
    treeModel.SetTextureTiling(1.0f);     
    farmhouseModel.SetTextureTiling(1.0f); 
    wallModel.SetTextureTiling(1.0f);
    lampModel.SetTextureTiling(1.0f);

    std::vector <Texture> tex(textures, textures + sizeof(textures) / sizeof(Texture));
    std::vector <Texture> tex2(textures, textures + sizeof(textures) / sizeof(Texture));

    // Model matrix for the terrain
    glm::mat4 terrainModelMatrix = glm::mat4(1.0f);
    terrainModelMatrix = glm::translate(terrainModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f)); 
    terrainModelMatrix = glm::scale(terrainModelMatrix, glm::vec3(100.0f, 100.0f, 100.0f));
    terrainModel.AddTexture(tex[0]);

    // Model matrix for the tree
    glm::mat4 treeModelMatrix = glm::mat4(1.0f);
    treeModelMatrix = glm::translate(treeModelMatrix, glm::vec3(-10.0f, 0.0f, -10.0f));
    treeModelMatrix = glm::scale(treeModelMatrix, glm::vec3(10.0f, 10.0f, 10.0f));


    // Model matrix for the farmhouse
    glm::mat4 farmhouseModelMatrix = glm::mat4(1.0f);
    farmhouseModelMatrix = glm::translate(farmhouseModelMatrix, glm::vec3(10.0f, 0.0f, 10.0f));
    farmhouseModelMatrix = glm::rotate(farmhouseModelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    farmhouseModelMatrix = glm::scale(farmhouseModelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));

    glm::mat4 wallModelMatrix = glm::mat4(1.0f);
    wallModelMatrix = glm::translate(wallModelMatrix, glm::vec3(1.0f, 5.0f, 1.0f));
    wallModelMatrix = glm::rotate(farmhouseModelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    wallModel.AddTexture(tex2[0]);

    // Model matrix for the lamp post
    glm::mat4 lampModelMatrix = glm::mat4(1.0f);
    lampModelMatrix = glm::translate(lampModelMatrix, glm::vec3(-5.0f, 0.0f, 20.0f)); // Position it near the center
    lampModelMatrix = glm::rotate(lampModelMatrix, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    lampModelMatrix = glm::scale(lampModelMatrix, glm::vec3(0.7f, 0.7f, 0.7f)); // Scale it down to appropriate size
    
    // Build collider for the lamp
    lampModel.buildCollider(lampModelMatrix);
    
    // Build component-specific colliders for the tree
    treeModel.buildComponentColliders(treeModelMatrix);
    
    // Get the trunk collider specifically (Trank_bark is the material name for the trunk)
    Collider* trunkCollider = treeModel.getComponentCollider("Trank_bark");
    
    // Build regular collider for farmhouse
    farmhouseModel.buildCollider(farmhouseModelMatrix);

    // Build a collider for the terrain
    terrainModel.buildCollider(terrainModelMatrix);

    // Add colliders to world colliders
    std::vector<Collider> worldColliders;
    
    // Add lamp collider to world colliders
    worldColliders.push_back(lampModel.collider);
    
    // Always use the trunk collider - if it exists, create a cylinder collider
    if (trunkCollider != nullptr) {
        std::cout << "Using cylinder collider for the tree trunk" << std::endl;
        
        // Get the original min/max points
        glm::vec3 originalMin = trunkCollider->getMin();
        glm::vec3 originalMax = trunkCollider->getMax();
        
        // Calculate the center position for the cylinder (base center)
        glm::vec3 trunkCenter = glm::vec3(
            (originalMin.x + originalMax.x) * 0.5f,  // Center X
            originalMin.y,                           // Base of trunk (Y)
            (originalMin.z + originalMax.z) * 0.5f   // Center Z
        );
        
        // Calculate cylinder dimensions
        float trunkHeight = originalMax.y - originalMin.y;
        
        // Calculate trunk radius (use a fraction of the width/depth of the bounding box)
        float boxWidth = originalMax.x - originalMin.x;
        float boxDepth = originalMax.z - originalMin.z;
        float trunkRadius = std::min(boxWidth, boxDepth) * 0.1f; 
        
        // Create a cylindrical collider
        Collider cylinderCollider(trunkCenter, trunkRadius, trunkHeight);
        
        // Debug output
        std::cout << "Original trunk box: min(" << originalMin.x << "," << originalMin.y << "," << originalMin.z 
                  << ") max(" << originalMax.x << "," << originalMax.y << "," << originalMax.z << ")" << std::endl;
        std::cout << "Cylinder collider: center(" << trunkCenter.x << "," << trunkCenter.y << "," << trunkCenter.z 
                  << ") radius=" << trunkRadius << " height=" << trunkHeight << std::endl;
        
        // Add the cylinder collider for the trunk
        worldColliders.push_back(cylinderCollider);
    } else {
        // If trunk collider not found, log a warning but don't add any tree collider
        std::cout << "WARNING: Trunk collider not found for tree, no tree collision will be applied" << std::endl;
    }
    
    worldColliders.push_back(farmhouseModel.collider);
    
    // Add the terrain collider to prevent going through it
    worldColliders.push_back(terrainModel.collider);
    
    glm::vec4 lightColor = glm::vec4(1.50f, 1.50f, 1.50f, 0.50f);
    glm::vec3 lightPos = glm::vec3(0.0f, 50.0f, 0.0f);
    glm::mat4 lightModelMatrix = glm::mat4(2.0f); // Fix: use identity matrix
    lightModelMatrix = glm::translate(lightModelMatrix, lightPos); // Translate to light position
    lightModelMatrix = glm::scale(lightModelMatrix, glm::vec3(100.0f, 100.0f, 100.0f)); // Reasonable scale for marker


    
    lightShader.Activate();
    glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModelMatrix));
    glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    shaderProgram.Activate();
    glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

    // Enables the Depth Buffer
    glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);

    // Initialize player with proper ground-level starting position
    Player player(width, height, glm::vec3(20.0f, 1.7f, 20.0f));
    player.speed = 10.0f;  // More realistic movement speed for first-person

    // Remplacer la création des lumières :
    std::vector<Light> sceneLights;

    // Directional light (sun)
    sceneLights.emplace_back(
        0, // type
        glm::vec3(0.0f), // position (inutile pour directionnelle)
        glm::vec3(-0.2f, -1.0f, -0.3f), // direction
        glm::vec4(1.0f, 1.0f, 0.9f, 1.0f), // color
        nullptr // pas de mesh pour le soleil
    );

    // Spot light (lamp) avec mesh associé
    sceneLights.emplace_back(
        2, // type
        glm::vec3(5.0f, 3.0f, 5.0f), // position (match the top of lamp post)
        glm::vec3(0.0f, -1.0f, 0.0f), // direction (points down)
        glm::vec4(1.0f, 0.9f, 0.7f, 1.0f), // warm light color
        nullptr // no mesh, we'll draw the actual lamp model
    );

    float sunStrength = 1.0f;
    float fadeSpeed = 0.1f; // Plus c'est grand, plus ça descend vite


    // Main while loop
    while (!glfwWindowShouldClose(window))
    {
        // Specify the color of the background
        glClearColor(0.3f, 0.1f, 0.1f, 1.0f); // Dark red/brown to contrast with the terrain
        // Clean the back buffer and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update du joueur
        float deltaTime = glfwGetTime(); 
        glfwSetTime(0); // Remettre à 0 pour le frame suivant
        player.Update(window, worldColliders, deltaTime);

        sunStrength = glm::max(sunStrength, 0.0f);
        sceneLights[0].color = glm::vec4(1.0f, 1.0f, 0.9f, 1.0f) * sunStrength;

        // Envoie le nombre de lumières au shader
        glUniform1i(glGetUniformLocation(shaderProgram.ID, "lightCount"), sceneLights.size());

        // Envoie chaque lumière au shader via la méthode de la classe
        for (int i = 0; i < sceneLights.size(); i++) {
            sceneLights[i].sendToShader(shaderProgram, i);
        }

        glm::vec4 dayColor = glm::vec4(1.4f, 1.1f, 0.8f, 1.0f);
        glm::vec4 nightColor = glm::vec4(0.1f, 0.2f, 0.4f, 1.0f);

        // Affichage du mesh associé à la lumière (exemple pour la lampe)
        glm::mat4 lightModelMatrix = glm::mat4(1.0f);
        lightModelMatrix = glm::translate(lightModelMatrix, sceneLights[1].position);
        lightModelMatrix = glm::scale(lightModelMatrix, glm::vec3(5.0f, 5.0f, 5.0f));
        sceneLights[1].drawMesh(lightShader, player.camera, lightModelMatrix);

        // Draw tree
        treeModel.Draw(shaderProgram, player.camera, treeModelMatrix);
        
        // Draw farmhouse with custom material properties
        farmhouseModel.Draw(shaderProgram, player.camera, farmhouseModelMatrix);

        // Draw lamp post
        lampModel.Draw(shaderProgram, player.camera, lampModelMatrix);

		// Draw terrain
		terrainModel.Draw(shaderProgram, player.camera, terrainModelMatrix);

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