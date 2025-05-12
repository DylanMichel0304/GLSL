#include"src/Mesh.h"
#include"src/model.h"
#include<iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "src/Player.h"
#include "src/Collider.h"
#include "src/Light.h"
#include "src/ParticleSystem.h"
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <cmath>
#include "src/tree_collider_utils.h"

// Define this before including stb_image.h
#define STB_IMAGE_IMPLEMENTATION
#include "import/stb/stb_image.h"

const unsigned int width = 1200;
const unsigned int height = 1200;

Vertex vertices[] = {
	Vertex{glm::vec3(-1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f), glm::vec2(0.0f, 0.0f)},
	Vertex{glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f), glm::vec2(0.0f, 1.0f)},
	Vertex{glm::vec3( 1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f), glm::vec2(1.0f, 1.0f)},
	Vertex{glm::vec3( 1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f), glm::vec2(1.0f, 0.0f)}
};

GLuint indices[] = { 0, 1, 2, 0, 2, 3 };

Vertex lightVertices[] = {
	Vertex{glm::vec3(-0.1f, -0.1f,  0.1f)}, Vertex{glm::vec3(-0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3( 0.1f, -0.1f, -0.1f)}, Vertex{glm::vec3( 0.1f, -0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f,  0.1f)}, Vertex{glm::vec3(-0.1f,  0.1f, -0.1f)},
	Vertex{glm::vec3( 0.1f,  0.1f, -0.1f)}, Vertex{glm::vec3( 0.1f,  0.1f,  0.1f)}
};

GLuint lightIndices[] = {
	0,1,2, 0,2,3, 0,4,7, 0,7,3, 3,7,6, 3,6,2,
	2,6,5, 2,5,1, 1,5,4, 1,4,0, 4,5,6, 4,6,7
};
struct SceneTree {
    glm::vec3 position;
    glm::vec3 scale;
};

// Utility function to draw all trees
void DrawTrees(const std::vector<SceneTree>& trees, Model& treeModel, Shader& shader, Camera& camera) {
    for (const auto& tree : trees) {
        glm::mat4 treeModelMatrix = glm::mat4(1.0f);
        treeModelMatrix = glm::translate(treeModelMatrix, tree.position);
        treeModelMatrix = glm::scale(treeModelMatrix, tree.scale);
        treeModel.Draw(shader, camera, treeModelMatrix);
    }
}

std::vector<SceneTree> trees = {
    // First tree
    { glm::vec3(-12.0f, 0.0f, -12.0f), glm::vec3(2.8f, 3.4f, 2.8f) },
    // Row 1 (z = -22)
    { glm::vec3(-9.0f, 0.0f, -22.0f), glm::vec3(3.3f, 2.6f, 3.3f) },
    { glm::vec3(-3.0f, 0.0f, -22.0f), glm::vec3(2.5f, 3.5f, 2.5f) },
    { glm::vec3(3.0f, 0.0f, -22.0f), glm::vec3(3.2f, 2.7f, 3.2f) },
    { glm::vec3(9.0f, 0.0f, -22.0f), glm::vec3(2.6f, 3.4f, 2.6f) },
    // Row 2 (z = -16)
    { glm::vec3(-15.0f, 0.0f, -16.0f), glm::vec3(3.4f, 2.5f, 3.4f) },
    { glm::vec3(-9.0f, 0.0f, -16.0f), glm::vec3(2.7f, 3.3f, 2.7f) },
    { glm::vec3(-3.0f, 0.0f, -16.0f), glm::vec3(3.5f, 2.6f, 3.5f) },
    { glm::vec3(3.0f, 0.0f, -16.0f), glm::vec3(2.5f, 3.5f, 2.5f) },
    { glm::vec3(9.0f, 0.0f, -16.0f), glm::vec3(3.3f, 2.7f, 3.3f) },
    // Row 3 (z = -10)
    { glm::vec3(-15.0f, 0.0f, -10.0f), glm::vec3(2.6f, 3.4f, 2.6f) },
    { glm::vec3(-9.0f, 0.0f, -10.0f), glm::vec3(3.4f, 2.5f, 3.4f) },
    { glm::vec3(-3.0f, 0.0f, -10.0f), glm::vec3(2.7f, 3.3f, 2.7f) },
    { glm::vec3(3.0f, 0.0f, -10.0f), glm::vec3(3.5f, 2.6f, 3.5f) },
    { glm::vec3(9.0f, 0.0f, -10.0f), glm::vec3(2.5f, 3.5f, 2.5f) }
};

// Helper function to initialize GLFW, create window, and load GLAD
GLFWwindow* InitWindow(int width, int height, const char* title) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (!window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return nullptr;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGL()) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwDestroyWindow(window);
		glfwTerminate();
		return nullptr;
	}
	glViewport(0, 0, width, height);
	return window;
}

int main() {
    
	GLFWwindow* window = InitWindow(width, height, "3D_game");
	if (!window) return -1;

	std::string texPath = "assets/textures/";
	Texture textures[] = { Texture((texPath + "herbe.png").c_str(), "diffuse", 0) };
    Texture bronzeTexture((texPath + "brick.png").c_str(), "diffuse", 0);

	Shader shaderProgram("shader/default.vert", "shader/default.frag");
	std::vector<Vertex> verts(vertices, vertices + 4);
	std::vector<GLuint> ind(indices, indices + 6);
	std::vector<Texture> tex(textures, textures + 1);
	Mesh floor(verts, ind, tex);

	Shader lightShader("shader/light.vert", "shader/light.frag");
	std::vector<Vertex> lightVerts(lightVertices, lightVertices + 8);
	std::vector<GLuint> lightInd(lightIndices, lightIndices + 36);
	Mesh lightMesh(lightVerts, lightInd, tex);

    Shader particleShader("shader/particle.vert", "shader/particle.frag");
    Texture smokeTexture("assets/textures/smoke.png", "diffuse", 0);
    ParticleSystem campfireSmoke(&particleShader, smokeTexture.ID);


	std::vector<Light> lights = {
		{0, glm::vec3(-1.0f), glm::vec3(-0.0f, -1.0f, -0.0f), glm::vec4(1.0f)}, // Directional light, mesh at origin
		{1, glm::vec3(-12.0f, 12.0f, 7.0f), glm::vec3(0.0f), glm::vec4(1.0f, 0.5f, 0.0f, 10.0f)}, // Point light
		{2, glm::vec3(20.0f, 3.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec4(0.0f, 0.5f, 1.0f, 2.0f)}  // Spot light
	};

    // Load the models
	Model terrainModel("assets/objects/plane.obj");
	terrainModel.SetTextureTiling(150.0f);
	glm::mat4 terrainModelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(40.0f));
	terrainModel.AddTexture(tex[0]);


    Model treeModel("assets/objects/Tree 02/Tree.obj");
    treeModel.SetTextureTiling(1.0f);
    glm::mat4 treeModelMatrix = glm::mat4(1.0f);
    treeModelMatrix = glm::translate(treeModelMatrix, glm::vec3(-12.0f, 0.0f, -12.0f));
    treeModelMatrix = glm::scale(treeModelMatrix, glm::vec3(2.8f, 3.4f, 2.8f)); 


    Model farmhouseModel("assets/textures/newhouse/farmhouse_obj.obj");
    farmhouseModel.SetTextureTiling(1.0f); 
    glm::mat4 farmhouseModelMatrix = glm::mat4(1.0f);
    farmhouseModelMatrix = glm::translate(farmhouseModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
    farmhouseModelMatrix = glm::rotate(farmhouseModelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    farmhouseModelMatrix = glm::scale(farmhouseModelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));

    Model lampModel("assets/objects/LAMP/rv_lamp_post_4.obj", true);
    lampModel.SetTextureTiling(1.0f);
    glm::mat4 lampModelMatrix = glm::mat4(1.0f);
    lampModelMatrix = glm::translate(lampModelMatrix, glm::vec3(-15.0f, 0.0f, 10.0f));
    lampModelMatrix = glm::rotate(lampModelMatrix, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    lampModelMatrix = glm::scale(lampModelMatrix, glm::vec3(0.7f, 0.7f, 0.7f));
    lampModel.AddTexture(bronzeTexture);
    
	Player player(width, height, glm::vec3(20.0f, 1.7f, 20.0f));
	player.speed = 10.0f;
	glEnable(GL_DEPTH_TEST);
	float time = 0.0f;

    // Only keep the first column of trees (i.e., those with the same x as the first tree)
    std::vector<glm::vec3> treePositions;
    if (!trees.empty()) {
        float firstX = trees[0].position.x;
        for (const auto& tree : trees) {
            if (tree.position.x == firstX) {
                treePositions.push_back(tree.position);
            }
        }
    }

    // Build component-specific colliders for the tree
    treeModel.buildComponentColliders(treeModelMatrix);
    Collider* trunkCollider = treeModel.getComponentCollider("Trank_bark");

    // Create tree colliders using the refactored function
    std::vector<Collider> treeColliders = CreateTreeColliders(trunkCollider, treePositions);

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		float deltaTime = glfwGetTime();
		glfwSetTime(0);
		time += deltaTime;
		player.Update(window, {}, deltaTime);

		// Animate lights
		lights[1].color = glm::vec4(2.0f, 1.0f, 0.0f, 1.0f);
		lights[2].color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f) * (0.5f + 0.5f * cos(time));
		lights[0].color = glm::vec4(1.0f) * (0.3f + 0.7f * abs(sin(time * 0.5f)));

		shaderProgram.Activate();
		glUniform1i(glGetUniformLocation(shaderProgram.ID, "lightCount"), lights.size());
		for (int i = 0; i < lights.size(); ++i) lights[i].sendToShader(shaderProgram, i);

		terrainModel.Draw(shaderProgram, player.camera, terrainModelMatrix);
        lampModel.Draw(shaderProgram, player.camera, lampModelMatrix);
        farmhouseModel.Draw(shaderProgram, player.camera, farmhouseModelMatrix);
        DrawTrees(trees, treeModel, shaderProgram, player.camera);

		lightShader.Activate();
		for (const auto& light : lights) {
			glm::mat4 lightModelMatrix = glm::translate(glm::mat4(1.0f), light.position);
			lightModelMatrix = glm::scale(lightModelMatrix, glm::vec3(1.0f)); // Increased scale
			glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModelMatrix));
			glUniform4fv(glGetUniformLocation(lightShader.ID, "lightColor"), 1, glm::value_ptr(light.color));
			lightMesh.Draw(lightShader, player.camera);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	shaderProgram.Delete();
	lightShader.Delete();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}