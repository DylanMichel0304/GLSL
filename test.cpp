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

    // Create the bronze texture for the lamp - using same syntax as other textures
    Texture bronzeTexture((texPath + "../objects/LAMP/bronze_preview.jpg").c_str(), "diffuse", 0);

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

    // Explicitly clear all textures from the lamp model and add the bronze texture to each mesh
    for (auto& mesh : lampModel.meshes) {
        mesh.textures.clear();
        mesh.textures.push_back(bronzeTexture);  // Add bronze texture to every mesh
    }

    // Set different texture tiling for each model
    terrainModel.SetTextureTiling(150.0f);  
    treeModel.SetTextureTiling(1.0f);     
    farmhouseModel.SetTextureTiling(1.0f); 
    wallModel.SetTextureTiling(1.0f);
    lampModel.SetTextureTiling(1.0f);  // Set tiling for bronze texture

    std::vector <Texture> tex(textures, textures + sizeof(textures) / sizeof(Texture));
    std::vector <Texture> tex2(textures, textures + sizeof(textures) / sizeof(Texture));
    // Create empty texture vector for the lamp to use only MTL colors
    std::vector<Texture> emptyTextures;

    // Model matrix for the terrain
    glm::mat4 terrainModelMatrix = glm::mat4(1.0f);
    terrainModelMatrix = glm::translate(terrainModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f)); 
    terrainModelMatrix = glm::scale(terrainModelMatrix, glm::vec3(20.0f, 25.0f, 25.0f));
    terrainModel.AddTexture(tex[0]);

    // Model matrix for the tree
    glm::mat4 treeModelMatrix = glm::mat4(1.0f);
    treeModelMatrix = glm::translate(treeModelMatrix, glm::vec3(-12.0f, 0.0f, -12.0f));
    treeModelMatrix = glm::scale(treeModelMatrix, glm::vec3(2.8f, 3.4f, 2.8f));


    // Model matrix for the farmhouse
    glm::mat4 farmhouseModelMatrix = glm::mat4(1.0f);
    farmhouseModelMatrix = glm::translate(farmhouseModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
    farmhouseModelMatrix = glm::rotate(farmhouseModelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    farmhouseModelMatrix = glm::scale(farmhouseModelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));

    glm::mat4 wallModelMatrix = glm::mat4(1.0f);
    wallModelMatrix = glm::translate(wallModelMatrix, glm::vec3(1.0f, 5.0f, 1.0f));
    wallModelMatrix = glm::rotate(farmhouseModelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    wallModel.AddTexture(tex2[0]);

    // Model matrix for the lamp post
    glm::mat4 lampModelMatrix = glm::mat4(1.0f);
    lampModelMatrix = glm::translate(lampModelMatrix, glm::vec3(-15.0f, 0.0f, 10.0f));
    lampModelMatrix = glm::rotate(lampModelMatrix, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    lampModelMatrix = glm::scale(lampModelMatrix, glm::vec3(0.7f, 0.7f, 0.7f));
    
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
        
        // Calculate cylinder dimensions
        float trunkHeight = originalMax.y - originalMin.y;
        float boxWidth = originalMax.x - originalMin.x;
        float boxDepth = originalMax.z - originalMin.z;
        float trunkRadius = std::min(boxWidth, boxDepth) * 0.1f;

        // Function to create a cylinder collider at a specific position
        auto createTreeCollider = [&](const glm::vec3& position) {
            glm::vec3 trunkCenter = glm::vec3(
                position.x,  // Center X
                position.y,  // Base of trunk (Y)
                position.z  // Center Z
            );
            return Collider(trunkCenter, trunkRadius, trunkHeight);
        };

        // Add colliders for all trees
        // First tree
        worldColliders.push_back(createTreeCollider(glm::vec3(-12.0f, 0.0f, -12.0f)));
        
        // Row 1 (z = -22)
        worldColliders.push_back(createTreeCollider(glm::vec3(-9.0f, 0.0f, -22.0f)));
        worldColliders.push_back(createTreeCollider(glm::vec3(-3.0f, 0.0f, -22.0f)));
        worldColliders.push_back(createTreeCollider(glm::vec3(3.0f, 0.0f, -22.0f)));
        worldColliders.push_back(createTreeCollider(glm::vec3(9.0f, 0.0f, -22.0f)));

        // Row 2 (z = -16)
        worldColliders.push_back(createTreeCollider(glm::vec3(-15.0f, 0.0f, -16.0f)));
        worldColliders.push_back(createTreeCollider(glm::vec3(-9.0f, 0.0f, -16.0f)));
        worldColliders.push_back(createTreeCollider(glm::vec3(-3.0f, 0.0f, -16.0f)));
        worldColliders.push_back(createTreeCollider(glm::vec3(3.0f, 0.0f, -16.0f)));
        worldColliders.push_back(createTreeCollider(glm::vec3(9.0f, 0.0f, -16.0f)));

        // Row 3 (z = -10)
        worldColliders.push_back(createTreeCollider(glm::vec3(-15.0f, 0.0f, -10.0f)));
        worldColliders.push_back(createTreeCollider(glm::vec3(-9.0f, 0.0f, -10.0f)));
        worldColliders.push_back(createTreeCollider(glm::vec3(-3.0f, 0.0f, -10.0f)));
        worldColliders.push_back(createTreeCollider(glm::vec3(3.0f, 0.0f, -10.0f)));
        worldColliders.push_back(createTreeCollider(glm::vec3(9.0f, 0.0f, -10.0f)));

        std::cout << "Added cylinder colliders for all 17 trees" << std::endl;
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

    float sunStrength = 0.9f;
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
        
        // Draw additional trees in a grid pattern
        // Row 1 (z = -22)
        glm::mat4 tree2ModelMatrix = glm::mat4(1.0f);
        tree2ModelMatrix = glm::translate(tree2ModelMatrix, glm::vec3(-9.0f, 0.0f, -22.0f));
        tree2ModelMatrix = glm::scale(tree2ModelMatrix, glm::vec3(3.3f, 2.6f, 3.3f));
        treeModel.Draw(shaderProgram, player.camera, tree2ModelMatrix);

        glm::mat4 tree3ModelMatrix = glm::mat4(1.0f);
        tree3ModelMatrix = glm::translate(tree3ModelMatrix, glm::vec3(-3.0f, 0.0f, -22.0f));
        tree3ModelMatrix = glm::scale(tree3ModelMatrix, glm::vec3(2.5f, 3.5f, 2.5f));
        treeModel.Draw(shaderProgram, player.camera, tree3ModelMatrix);

        glm::mat4 tree4ModelMatrix = glm::mat4(1.0f);
        tree4ModelMatrix = glm::translate(tree4ModelMatrix, glm::vec3(3.0f, 0.0f, -22.0f));
        tree4ModelMatrix = glm::scale(tree4ModelMatrix, glm::vec3(3.2f, 2.7f, 3.2f));
        treeModel.Draw(shaderProgram, player.camera, tree4ModelMatrix);

        glm::mat4 tree5ModelMatrix = glm::mat4(1.0f);
        tree5ModelMatrix = glm::translate(tree5ModelMatrix, glm::vec3(9.0f, 0.0f, -22.0f));
        tree5ModelMatrix = glm::scale(tree5ModelMatrix, glm::vec3(2.6f, 3.4f, 2.6f));
        treeModel.Draw(shaderProgram, player.camera, tree5ModelMatrix);

        // Row 2 (z = -16)
        glm::mat4 tree6ModelMatrix = glm::mat4(1.0f);
        tree6ModelMatrix = glm::translate(tree6ModelMatrix, glm::vec3(-15.0f, 0.0f, -16.0f));
        tree6ModelMatrix = glm::scale(tree6ModelMatrix, glm::vec3(3.4f, 2.5f, 3.4f));
        treeModel.Draw(shaderProgram, player.camera, tree6ModelMatrix);

        glm::mat4 tree7ModelMatrix = glm::mat4(1.0f);
        tree7ModelMatrix = glm::translate(tree7ModelMatrix, glm::vec3(-9.0f, 0.0f, -16.0f));
        tree7ModelMatrix = glm::scale(tree7ModelMatrix, glm::vec3(2.7f, 3.3f, 2.7f));
        treeModel.Draw(shaderProgram, player.camera, tree7ModelMatrix);

        glm::mat4 tree8ModelMatrix = glm::mat4(1.0f);
        tree8ModelMatrix = glm::translate(tree8ModelMatrix, glm::vec3(-3.0f, 0.0f, -16.0f));
        tree8ModelMatrix = glm::scale(tree8ModelMatrix, glm::vec3(3.5f, 2.6f, 3.5f));
        treeModel.Draw(shaderProgram, player.camera, tree8ModelMatrix);

        glm::mat4 tree9ModelMatrix = glm::mat4(1.0f);
        tree9ModelMatrix = glm::translate(tree9ModelMatrix, glm::vec3(3.0f, 0.0f, -16.0f));
        tree9ModelMatrix = glm::scale(tree9ModelMatrix, glm::vec3(2.5f, 3.5f, 2.5f));
        treeModel.Draw(shaderProgram, player.camera, tree9ModelMatrix);

        glm::mat4 tree10ModelMatrix = glm::mat4(1.0f);
        tree10ModelMatrix = glm::translate(tree10ModelMatrix, glm::vec3(9.0f, 0.0f, -16.0f));
        tree10ModelMatrix = glm::scale(tree10ModelMatrix, glm::vec3(3.3f, 2.7f, 3.3f));
        treeModel.Draw(shaderProgram, player.camera, tree10ModelMatrix);

        // Row 3 (z = -10)
        glm::mat4 tree11ModelMatrix = glm::mat4(1.0f);
        tree11ModelMatrix = glm::translate(tree11ModelMatrix, glm::vec3(-15.0f, 0.0f, -10.0f));
        tree11ModelMatrix = glm::scale(tree11ModelMatrix, glm::vec3(2.6f, 3.4f, 2.6f));
        treeModel.Draw(shaderProgram, player.camera, tree11ModelMatrix);

        glm::mat4 tree12ModelMatrix = glm::mat4(1.0f);
        tree12ModelMatrix = glm::translate(tree12ModelMatrix, glm::vec3(-9.0f, 0.0f, -10.0f));
        tree12ModelMatrix = glm::scale(tree12ModelMatrix, glm::vec3(3.4f, 2.5f, 3.4f));
        treeModel.Draw(shaderProgram, player.camera, tree12ModelMatrix);

        glm::mat4 tree13ModelMatrix = glm::mat4(1.0f);
        tree13ModelMatrix = glm::translate(tree13ModelMatrix, glm::vec3(-3.0f, 0.0f, -10.0f));
        tree13ModelMatrix = glm::scale(tree13ModelMatrix, glm::vec3(2.7f, 3.3f, 2.7f));
        treeModel.Draw(shaderProgram, player.camera, tree13ModelMatrix);

        glm::mat4 tree14ModelMatrix = glm::mat4(1.0f);
        tree14ModelMatrix = glm::translate(tree14ModelMatrix, glm::vec3(3.0f, 0.0f, -10.0f));
        tree14ModelMatrix = glm::scale(tree14ModelMatrix, glm::vec3(3.5f, 2.6f, 3.5f));
        treeModel.Draw(shaderProgram, player.camera, tree14ModelMatrix);

        glm::mat4 tree15ModelMatrix = glm::mat4(1.0f);
        tree15ModelMatrix = glm::translate(tree15ModelMatrix, glm::vec3(9.0f, 0.0f, -10.0f));
        tree15ModelMatrix = glm::scale(tree15ModelMatrix, glm::vec3(2.5f, 3.5f, 2.5f));
        treeModel.Draw(shaderProgram, player.camera, tree15ModelMatrix);

       

        // Draw farmhouse with custom material properties
        farmhouseModel.Draw(shaderProgram, player.camera, farmhouseModelMatrix);

        // Reset material properties to default before drawing the lamp
        shaderProgram.Activate();
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "material.ambient"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "material.diffuse"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "material.specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(shaderProgram.ID, "material.shininess"), 32.0f);
        
        // Draw lamp post with bronze texture
        lampModel.Draw(shaderProgram, player.camera, lampModelMatrix);

        // Reset material properties again
        shaderProgram.Activate();
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "material.ambient"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "material.diffuse"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "material.specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(shaderProgram.ID, "material.shininess"), 32.0f);
        
        // Set terrain material properties to increase luminosity (brightness)
        shaderProgram.Activate();
        // Increase ambient to brighten the texture in shadowed areas (values > 1.0 increase brightness)
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "material.ambient"), 1.0f, 1.0f, 1.0f);
        // Increase diffuse to brighten the texture in lit areas
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "material.diffuse"), 1.0f, 1.0f, 1.0f);
        // Adjust specular for highlights
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "material.specular"), 0.5f, 0.5f, 0.5f);
        glUniform1f(glGetUniformLocation(shaderProgram.ID, "material.shininess"), 10.0f);

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