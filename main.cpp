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
#include "src/Cubemaps.h"
#include "src/Campfire.h"

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
// Represents a tree instance in the scene with position and scale
struct SceneTree {
    glm::vec3 position; // World position of the tree
    glm::vec3 scale;    // Scale of the tree
};

// Utility function to draw all trees in the scene
// Iterates over the list of SceneTree and draws each tree model at its position and scale
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
	Texture textures[] = { Texture((texPath + "planks.png").c_str(), "diffuse", 0),
                        Texture((texPath + "planksSpec.png").c_str(), "specular", 1)
                        };  
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

    Shader reflectionShader("shader/reflect.vert", "shader/reflect.frag");
    Shader refractionShader("shader/refrac.vert", "shader/refrac.frag");

    // --- Mirror geometry setup ---
    // Simple quad for mirrors (XZ plane, centered at origin)
    std::vector<Vertex> mirrorVertices = {
        Vertex{glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(0,1,0), glm::vec3(1.0f), glm::vec2(0,0)},
        Vertex{glm::vec3( 1.0f, 0.0f, -1.0f), glm::vec3(0,1,0), glm::vec3(1.0f), glm::vec2(1,0)},
        Vertex{glm::vec3( 1.0f, 0.0f,  1.0f), glm::vec3(0,1,0), glm::vec3(1.0f), glm::vec2(1,1)},
        Vertex{glm::vec3(-1.0f, 0.0f,  1.0f), glm::vec3(0,1,0), glm::vec3(1.0f), glm::vec2(0,1)}
    };
    std::vector<GLuint> mirrorIndices = {0,1,2, 0,2,3};
    std::vector<Texture> emptyTex; // No texture needed for mirror
    Mesh mirrorMesh(mirrorVertices, mirrorIndices, emptyTex);

    // Mirror 1: Reflection (vertical, like a wall)
    glm::mat4 mirror1Model = glm::mat4(1.0f);
    mirror1Model = glm::translate(mirror1Model, glm::vec3(- 10.0f, 1.0f, 0.0f)); // position
    mirror1Model = glm::rotate(mirror1Model, glm::radians(90.0f), glm::vec3(1,0,0)); // vertical
    mirror1Model = glm::scale(mirror1Model, glm::vec3(10.0f, 10.0f, 10.0f)); // size

    // Mirror 2: Refraction (horizontal, like a pool)
    glm::mat4 mirror2Model = glm::mat4(1.0f);
    mirror2Model = glm::translate(mirror2Model, glm::vec3(-5.0f, 0.2f, 0.0f)); // position
    mirror1Model = glm::rotate(mirror1Model, glm::radians(90.0f), glm::vec3(1,0,0)); // vertical
    mirror2Model = glm::scale(mirror2Model, glm::vec3(10.0f, 10.0f, 10.0f)); // size


	std::vector<Light> lights = {
		{0, glm::vec3(-1.0f), glm::vec3(-0.0f, -1.0f, -0.0f), glm::vec4(1.0f)}, // Directional light, mesh at origin
		{1, glm::vec3(-12.0f, 12.0f, 7.0f), glm::vec3(0.0f), glm::vec4(1.0f, 0.5f, 0.0f, 10.0f)}, // Point light
		{2, glm::vec3(20.0f, 3.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec4(0.0f, 0.5f, 1.0f, 2.0f)},  // Spot light
		{1, glm::vec3(5.0f, 1.0f, 0.0f), glm::vec3(0.0f), glm::vec4(1.0f, 0.7f, 0.2f, 5.0f)}  // Campfire light
	};
    std::vector<Collider> worldColliders;

    // Load the models/objects
	Model terrainModel("assets/objects/plane.obj");
	terrainModel.SetTextureTiling(150.0f);
	glm::mat4 terrainModelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(40.0f));
	terrainModel.AddTexture(tex[0]);


    Model treeModel("assets/objects/Tree 02/Tree.obj");
    treeModel.SetTextureTiling(1.0f);
    glm::mat4 treeModelMatrix = glm::mat4(1.0f);
    treeModelMatrix = glm::translate(treeModelMatrix, glm::vec3(-12.0f, 0.0f, -12.0f));
    treeModelMatrix = glm::scale(treeModelMatrix, glm::vec3(2.8f, 3.4f, 2.8f));
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

    for (const auto& tree : trees) {
        treePositions.push_back(tree.position);
    }

    // Create all tree colliders (one per tree)
    std::vector<Collider> treeColliders = CreateTreeColliders(trunkCollider, treePositions);

    // Add all tree colliders to worldColliders
    worldColliders.insert(worldColliders.end(), treeColliders.begin(), treeColliders.end());

    Model farmhouseModel("assets/textures/newhouse/farmhouse_obj.obj");
    farmhouseModel.SetTextureTiling(1.0f); 
    glm::mat4 farmhouseModelMatrix = glm::mat4(1.0f);
    farmhouseModelMatrix = glm::translate(farmhouseModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
    farmhouseModelMatrix = glm::rotate(farmhouseModelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    farmhouseModelMatrix = glm::scale(farmhouseModelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
    farmhouseModel.buildCollider(farmhouseModelMatrix);
    worldColliders.push_back(farmhouseModel.collider);

    Model lampModel("assets/objects/LAMP/rv_lamp_post_4.obj", true);
    lampModel.SetTextureTiling(1.0f);
    glm::mat4 lampModelMatrix = glm::mat4(1.0f);
    lampModelMatrix = glm::translate(lampModelMatrix, glm::vec3(-15.0f, 0.0f, 10.0f));
    lampModelMatrix = glm::rotate(lampModelMatrix, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    lampModelMatrix = glm::scale(lampModelMatrix, glm::vec3(0.7f, 0.7f, 0.7f));
    lampModel.AddTexture(bronzeTexture);
    glm::vec3 lampBaseCenter = glm::vec3(-15.0f, 0.0f, 10.0f); // Base center of the lamp
    float lampRadius = 0.6f; // Radius of the cylinder
    float lampHeight = 2.5f; // Height of the cylinder
    Collider lampCollider(lampBaseCenter, lampRadius, lampHeight);
    worldColliders.push_back(lampCollider);
    
	Player player(width, height, glm::vec3(-15.0f, 1.7f, 15.0f));
	player.speed = 10.0f;
	glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	float time = 0.0f;


    // --- Cubemap setup ---
    const std::string skyboxFaces[6] = {
        "assets/cubesmaps/night/right.jpg",
        "assets/cubesmaps/night/left.jpg",
        "assets/cubesmaps/night/top.jpg",
        "assets/cubesmaps/night/bottom.jpg",
        "assets/cubesmaps/night/front.jpg",
        "assets/cubesmaps/night/back.jpg"
    };
    Cubemaps skybox(skyboxFaces, "shader/skybox.vert", "shader/skybox.frag");

    // Enable blending for skybox alpha
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	
	// Campfire growth parameters
	float growthStartTime = 5.0f;    // Start growing after 5 seconds
	float growthDuration = 20.0f;    // Take 20 seconds to reach full size
	float minScale = 1.0f;          // Starting scale
	float maxScale = 12.0f;         // Target scale
	bool growthComplete = false;

	// Create campfire at specific position (start with small scale)
	Campfire campfire(glm::vec3(5.0f, 0.05f, 0.0f), minScale);

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		float deltaTime = glfwGetTime();
		glfwSetTime(0);
		time += deltaTime;
		
		// Handle campfire growth
		if (!growthComplete && time > growthStartTime) {
			float growthProgress = (time - growthStartTime) / growthDuration;
			if (growthProgress >= 1.0f) {
				growthProgress = 1.0f;
				growthComplete = true;
			}
			
			// Calculate current scale using smooth interpolation
			float currentScale = minScale + (maxScale - minScale) * 
			                     (growthProgress * growthProgress * (3.0f - 2.0f * growthProgress)); // Smooth step
			                     
			// Update campfire scale
			campfire.SetScale(currentScale);
		}
		player.Update(window, worldColliders, deltaTime);

		lights[1].color = glm::vec4(2.0f, 1.0f, 0.0f, 1.0f);
		lights[2].color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f) * (0.5f + 0.5f * cos(time));
		lights[0].color = glm::vec4(1.0f) * (0.3f + 0.7f * abs(sin(time * 0.5f)));
		
		// Animate campfire light with flickering effect
		lights[3].color = glm::vec4(1.0f, 0.7f, 0.2f, 1.0f) * (0.8f + 0.2f * sin(time * 10.0f));

		shaderProgram.Activate();
		glUniform1i(glGetUniformLocation(shaderProgram.ID, "lightCount"), lights.size());
		for (int i = 0; i < lights.size(); ++i) lights[i].sendToShader(shaderProgram, i);

        glUseProgram(shaderProgram.ID);
        glUniform1i(glGetUniformLocation(shaderProgram.ID, "cubemapSampler"), 3);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.getCubemapID());

        glUniform1f(glGetUniformLocation(shaderProgram.ID, "reflectivity"), 0.0f);
		terrainModel.Draw(shaderProgram, player.camera, terrainModelMatrix);
        glUniform1f(glGetUniformLocation(shaderProgram.ID, "reflectivity"), 0.2f);
        lampModel.Draw(shaderProgram, player.camera, lampModelMatrix);
        glUniform1f(glGetUniformLocation(shaderProgram.ID, "reflectivity"), 0.0f);
        farmhouseModel.Draw(shaderProgram, player.camera, farmhouseModelMatrix);
        DrawTrees(trees, treeModel, shaderProgram, player.camera);

        // --- Draw Mirror 1 (Reflection) ---
        reflectionShader.Activate();

        // Passe les uniforms
        glUniformMatrix4fv(glGetUniformLocation(reflectionShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(mirror1Model));
        glUniform3fv(glGetUniformLocation(reflectionShader.ID, "u_view_pos"), 1, glm::value_ptr(player.camera.Position));
        glUniform1i(glGetUniformLocation(reflectionShader.ID, "cubemapSampler"), 0);

        // Active la texture cubemap de skybox
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.getCubemapID()); // <- nécessite une méthode getter

        // Matrice de vue/projection
        player.camera.Matrix(reflectionShader, "camMatrix");

        // Dessine le miroir
        mirrorMesh.Draw(reflectionShader, player.camera);

        // --- Draw Mirror 2 (Refraction) ---
        refractionShader.Activate();

        glUniformMatrix4fv(glGetUniformLocation(refractionShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(mirror2Model));
        glUniform3fv(glGetUniformLocation(refractionShader.ID, "u_view_pos"), 1, glm::value_ptr(player.camera.Position));
        glUniform1f(glGetUniformLocation(refractionShader.ID, "refractionIndice"), 1.52f); // Verre standard
        glUniform1i(glGetUniformLocation(refractionShader.ID, "cubemapSampler"), 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.getCubemapID());

        player.camera.Matrix(refractionShader, "camMatrix");
        mirrorMesh.Draw(refractionShader, player.camera);

        // Update and draw the campfire
        campfire.Update(deltaTime);
        campfire.Draw(player.camera);

		lightShader.Activate();
		for (int i = 0; i < lights.size(); ++i) {
			// Skip rendering the light mesh for the campfire light (index 3)
			if (i == 3) continue;
			
			glm::mat4 lightModelMatrix = glm::translate(glm::mat4(1.0f), lights[i].position);
			lightModelMatrix = glm::scale(lightModelMatrix, glm::vec3(1.0f)); // Increased scale
			glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModelMatrix));
			glUniform4fv(glGetUniformLocation(lightShader.ID, "lightColor"), 1, glm::value_ptr(lights[i].color));
			lightMesh.Draw(lightShader, player.camera);
		}

        float animated = 0.3f + 0.7f * std::abs(std::sin(time * 0.5f));
        float skyboxAlpha = glm::clamp(animated, 0.2f, 1.0f);
        skybox.setAlpha(skyboxAlpha);
        skybox.Draw(player.camera, width, height);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	shaderProgram.Delete();
	lightShader.Delete();
	skybox.Delete();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}