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
#include "stb_image.h"
const unsigned int width = 800;
const unsigned int height = 800;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "3D_game", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, width, height);

    Shader shaderProgram("shader/default.vert", "shader/default.frag");
    Shader lightShader("shader/light.vert", "shader/light.frag");

    Model terrainModel("assets/objects/plane.obj");
    Model treeModel("assets/objects/Tree 02/Tree.obj");
    Model farmhouseModel("assets/textures/newhouse/farmhouse_obj.obj");
    Model lampModel("assets/objects/LAMP/rv_lamp_post_4.obj", true);
    Model lightSphereModel("assets/objects/sphere.obj");

    Texture bronzeTexture("assets/objects/LAMP/bronze_preview.jpg", "diffuse", 1);
    for (auto& mesh : lampModel.meshes) {
        mesh.textures.clear();
        mesh.textures.push_back(bronzeTexture);
    }

    terrainModel.SetTextureTiling(150.0f);
    treeModel.SetTextureTiling(1.0f);
    farmhouseModel.SetTextureTiling(1.0f);
    lampModel.SetTextureTiling(1.0f);

    glm::mat4 terrainModelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(100.0f));
    glm::mat4 treeModelMatrix = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 0.0f, -10.0f)), glm::vec3(10.0f));
    glm::mat4 farmhouseModelMatrix = glm::scale(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 0.0f, 10.0f)), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)), glm::vec3(0.5f));
    glm::mat4 lampModelMatrix = glm::scale(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, 20.0f)), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f)), glm::vec3(0.7f));

    terrainModel.buildCollider(terrainModelMatrix);
    treeModel.buildComponentColliders(treeModelMatrix);
    farmhouseModel.buildCollider(farmhouseModelMatrix);
    lampModel.buildCollider(lampModelMatrix);

    std::vector<Collider> worldColliders = {
        terrainModel.collider,
        farmhouseModel.collider,
        lampModel.collider
    };

    Collider* trunkCollider = treeModel.getComponentCollider("Trank_bark");
    if (trunkCollider)
    {
        glm::vec3 min = trunkCollider->getMin();
        glm::vec3 max = trunkCollider->getMax();
        glm::vec3 center = glm::vec3((min.x + max.x) * 0.5f, min.y, (min.z + max.z) * 0.5f);
        float height = max.y - min.y;
        float radius = std::min(max.x - min.x, max.z - min.z) * 0.1f;
        worldColliders.emplace_back(center, radius, height);
    }

    Player player(width, height, glm::vec3(20.0f, 1.7f, 20.0f));
    player.speed = 10.0f;

    std::vector<Light> sceneLights = {
        {0, glm::vec3(0.0f), glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec4(1.0f), nullptr},
        {1, glm::vec3(-3.0f, 0.5f, 18.0f), glm::vec3(0.0f), glm::vec4(1.0f), nullptr},
        {2, glm::vec3(-5.0f, 0.5f, 20.0f), glm::vec3(0.0f, -0.4f, 0.0f), glm::vec4(2.0f), nullptr}
    };

    float timeOfDay = 0.0f;
    float minStrength = 0.15f;
    float maxStrength = 1.0f;
    float dayNightSpeed = 0.15f;

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.3f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float deltaTime = glfwGetTime();
        glfwSetTime(0);
        player.Update(window, worldColliders, deltaTime);

        timeOfDay += deltaTime * dayNightSpeed;
        if (timeOfDay > 2 * 3.14159f) timeOfDay -= 2 * 3.14159f;

        float cycle = (sin(timeOfDay) + 1.0f) * 0.5f;
        float sunStrength = glm::mix(minStrength, maxStrength, cycle);
        float nightFactor = 1.0f - cycle;

        sceneLights[0].color = glm::vec4(1.0f, 1.0f, 0.9f, 1.0f) * sunStrength;
        sceneLights[1].color = glm::vec4(1.0f, 0.6f, 0.4f, 1.0f) * nightFactor;
        sceneLights[2].color = glm::vec4(1.0f, 0.9f, 0.7f, 1.0f) * nightFactor;

        shaderProgram.Activate();
        glUniform1i(glGetUniformLocation(shaderProgram.ID, "lightCount"), sceneLights.size());
        for (int i = 0; i < sceneLights.size(); i++) {
            sceneLights[i].sendToShader(shaderProgram, i);
        }

        treeModel.Draw(shaderProgram, player.camera, treeModelMatrix);
        farmhouseModel.Draw(shaderProgram, player.camera, farmhouseModelMatrix);
        lampModel.Draw(shaderProgram, player.camera, lampModelMatrix);
        terrainModel.Draw(shaderProgram, player.camera, terrainModelMatrix);

        glm::mat4 lightMat = glm::scale(glm::translate(glm::mat4(1.0f), sceneLights[2].position), glm::vec3(3.0f));
        lightSphereModel.Draw(lightShader, player.camera, lightMat);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    shaderProgram.Delete();
    lightShader.Delete();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
