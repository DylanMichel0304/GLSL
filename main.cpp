#include"src/Mesh.h"
#include"src/model.h"
#include<iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "src/Player.h"
#include "src/Collider.h"
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


const unsigned int width = 800;
const unsigned int height = 800;


// Ajout de la structure Light et du vecteur sceneLights
struct Light {
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec4 color;
    int type; // 0 = Directional, 1 = Point, 2 = Spot
};

std::vector<Light> sceneLights;


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

    // Model matrix for the terrain
    glm::mat4 terrainModelMatrix = glm::mat4(1.0f);
    terrainModelMatrix = glm::translate(terrainModelMatrix, glm::vec3(0.0f, -20.0f, 0.0f)); 
    terrainModelMatrix = glm::scale(terrainModelMatrix, glm::vec3(100.0f, 100.0f, 100.0f));

    // Model matrix for the tree
    glm::mat4 treeModelMatrix = glm::mat4(1.0f);
    treeModelMatrix = glm::translate(treeModelMatrix, glm::vec3(20.0f, -10.0f, 0.0f));
    treeModelMatrix = glm::scale(treeModelMatrix, glm::vec3(10.0f, 10.0f, 10.0f));

    // Model matrix for the farmhouse
    glm::mat4 farmhouseModelMatrix = glm::mat4(1.0f);
    farmhouseModelMatrix = glm::translate(farmhouseModelMatrix, glm::vec3(40.0f, -10.0f, 10.0f));
    farmhouseModelMatrix = glm::rotate(farmhouseModelMatrix, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    farmhouseModelMatrix = glm::scale(farmhouseModelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));

    // Build colliders for models
    treeModel.buildCollider(treeModelMatrix);
    farmhouseModel.buildCollider(farmhouseModelMatrix);

    // Add colliders to world colliders
    std::vector<Collider> worldColliders;
    worldColliders.push_back(treeModel.collider);
    worldColliders.push_back(farmhouseModel.collider);


    // Material properties for farmhouse
    glm::vec3 farmhouseMaterial[] = {
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f), 
        glm::vec3(1.0f, 1.0f, 1.0f)  
    };
    
    glm::vec4 lightColor = glm::vec4(1.50f, 1.50f, 1.50f, 0.50f);
    glm::vec3 lightPos = glm::vec3(0.0f, 50.0f, 0.0f);
    glm::mat4 lightModelMatrix = glm::mat4(2.0f); // Fix: use identity matrix
    lightModelMatrix = glm::translate(lightModelMatrix, lightPos); // Translate to light position
    lightModelMatrix = glm::scale(lightModelMatrix, glm::vec3(100.0f, 100.0f, 100.0f)); // Reasonable scale for marker

    // Default tiling values for different objects
    float terrainTiling = 50.0f;  
    float treeTiling = 1.0f;       
    float farmhouseTiling = 1.0f;  
    
    lightShader.Activate();
    glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModelMatrix));
    glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    shaderProgram.Activate();
    glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

    // Enables the Depth Buffer
    glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);

    Player player(width, height, glm::vec3(0.0f, 20.0f, 50.0f));
    player.camera.speed = 10.0f;

    // Initialisation des lumières
    Light sun;
    sun.type = 0; // Directional
    sun.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    sun.color = glm::vec4(1.0f, 1.0f, 0.9f, 1.0f); // Soleil jaune
    sceneLights.push_back(sun);

    Light lamp;
    lamp.type = 2; // Spot
    lamp.position = glm::vec3(0.0f, 50.0f, 0.0f);
    lamp.direction = glm::vec3(0.0f, -1.0f, 0.0f);
    lamp.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // Lampe blanche
    sceneLights.push_back(lamp);

    float sunStrength = 10.0f;
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

        // Gestion de la lumière du soleil (diminution progressive)
        if (sunStrength > 0.0f)
            sunStrength -= fadeSpeed * deltaTime;
        sunStrength = glm::max(sunStrength, 0.0f);
        sceneLights[0].color = glm::vec4(1.0f, 1.0f, 0.9f, 1.0f) * sunStrength;

        // Envoie le nombre de lumières au shader
        glUniform1i(glGetUniformLocation(shaderProgram.ID, "lightCount"), sceneLights.size());

        // Envoie chaque lumière au shader
        for (int i = 0; i < sceneLights.size(); i++) {
            std::string number = std::to_string(i);
            glUniform3fv(glGetUniformLocation(shaderProgram.ID, ("lights[" + number + "].position").c_str()), 1, glm::value_ptr(sceneLights[i].position));
            glUniform3fv(glGetUniformLocation(shaderProgram.ID, ("lights[" + number + "].direction").c_str()), 1, glm::value_ptr(sceneLights[i].direction));
            glUniform4fv(glGetUniformLocation(shaderProgram.ID, ("lights[" + number + "].color").c_str()), 1, glm::value_ptr(sceneLights[i].color));
            glUniform1i(glGetUniformLocation(shaderProgram.ID, ("lights[" + number + "].type").c_str()), sceneLights[i].type);
        }

        glm::vec4 dayColor = glm::vec4(1.4f, 1.1f, 0.8f, 1.0f);
        glm::vec4 nightColor = glm::vec4(0.1f, 0.2f, 0.4f, 1.0f);

        // Update light model matrix for the sphere marker
        lightModelMatrix = glm::mat4(1.0f);
        lightModelMatrix = glm::translate(lightModelMatrix, lightPos);
        lightModelMatrix = glm::scale(lightModelMatrix, glm::vec3(5.0f, 5.0f, 5.0f)); // Increased size


        // Draw the light marker (sphere)
        lightShader.Activate();
        glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModelMatrix));
        glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
        //light.Draw(lightShader, player.camera);



        shaderProgram.Activate();
        glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
        
        // Draw terrain
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(terrainModelMatrix));
        glUniform1f(glGetUniformLocation(shaderProgram.ID, "textureTiling"), terrainTiling);
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "material.ambient"), 0.2f, 0.2f, 0.2f);
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "material.diffuse"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "material.specular"), 0.5f, 0.5f, 0.5f);
        glUniform1f(glGetUniformLocation(shaderProgram.ID, "material.shininess"), 32.0f);
        terrainModel.Draw(shaderProgram, player.camera);
        
        // Draw tree
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(treeModelMatrix));
        glUniform1f(glGetUniformLocation(shaderProgram.ID, "textureTiling"), treeTiling);
        treeModel.Draw(shaderProgram, player.camera);
        
        // Draw farmhouse with custom material properties
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(farmhouseModelMatrix));
        glUniform1f(glGetUniformLocation(shaderProgram.ID, "textureTiling"), farmhouseTiling);
        glUniform3fv(glGetUniformLocation(shaderProgram.ID, "material.ambient"), 1, glm::value_ptr(farmhouseMaterial[0]));
        glUniform3fv(glGetUniformLocation(shaderProgram.ID, "material.diffuse"), 1, glm::value_ptr(farmhouseMaterial[1]));
        glUniform3fv(glGetUniformLocation(shaderProgram.ID, "material.specular"), 1, glm::value_ptr(farmhouseMaterial[2]));
        glUniform1f(glGetUniformLocation(shaderProgram.ID, "material.shininess"), 32.0f);
        farmhouseModel.Draw(shaderProgram, player.camera);

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