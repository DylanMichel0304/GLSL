#include"src/Mesh.h"
#include"src/model.h"
#include<iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


const unsigned int width = 800;
const unsigned int height = 800;

// Utility function to generate a UV sphere
void generateSphere(float radius, unsigned int sectorCount, unsigned int stackCount, std::vector<Vertex>& vertices, std::vector<GLuint>& indices) {
    float x, y, z, xy;                              // vertex position
    float sectorStep = 2 * M_PI / sectorCount;
    float stackStep = M_PI / stackCount;
    float sectorAngle, stackAngle;

    for(unsigned int i = 0; i <= stackCount; ++i) {
        stackAngle = M_PI / 2 - i * stackStep;        // from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        for(unsigned int j = 0; j <= sectorCount; ++j) {
            sectorAngle = j * sectorStep;           // from 0 to 2pi
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            vertices.push_back(Vertex{glm::vec3(x, y, z)});
        }
    }

    for(unsigned int i = 0; i < stackCount; ++i) {
        unsigned int k1 = i * (sectorCount + 1);     // beginning of current stack
        unsigned int k2 = k1 + sectorCount + 1;      // beginning of next stack

        for(unsigned int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            if(i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }
            if(i != (stackCount-1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
}

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
		Texture((texPath + "newgrass.jpg").c_str(), "diffuse", 0, GL_RGBA, GL_UNSIGNED_BYTE),
		//Texture((texPath + "planksSpec.png").c_str(), "specular", 1, GL_RED, GL_UNSIGNED_BYTE)
	};



	// Generates Shader object using shaders default.vert and default.frag
	Shader shaderProgram("default.vert", "default.frag");
	// Store mesh data in vectors for the mesh
	std::vector <Texture> tex(textures, textures + sizeof(textures) / sizeof(Texture));


	// Shader for light cube
	Shader lightShader("light.vert", "light.frag");

	// Store mesh data in vectors for the mesh
	std::vector<Vertex> lightVerts;
	std::vector<GLuint> lightInd;
	generateSphere(15.0f, 32, 16, lightVerts, lightInd);

	// Create light mesh (sphere)
	Mesh light(lightVerts, lightInd, tex);

	// Load terrain model
	Model terrainModel("assets/objects/terrain.obj");
	std::cout << "Loaded terrain model successfully" << std::endl;
	
	// Load the tree model
	Model treeModel("assets/objects/Tree 02/Tree.obj");
	std::cout << "Loaded tree model successfully" << std::endl;

	glm::vec4 lightColor = glm::vec4(1.50f, 1.50f, 1.50f, 0.50f);
	glm::vec3 lightPos = glm::vec3(0.0f, 50.0f, 0.0f);
	glm::mat4 lightModelMatrix = glm::mat4(2.0f); // Fix: use identity matrix
	lightModelMatrix = glm::translate(lightModelMatrix, lightPos); // Translate to light position
	lightModelMatrix = glm::scale(lightModelMatrix, glm::vec3(100.0f, 100.0f, 100.0f)); // Reasonable scale for marker

	// Model matrix for the terrain
	glm::mat4 terrainModelMatrix = glm::mat4(1.0f);
	// Position the terrain lower (negative Y) since terrain is typically below the camera
	terrainModelMatrix = glm::translate(terrainModelMatrix, glm::vec3(0.0f, -20.0f, 0.0f)); 
	// Make the terrain much larger
	terrainModelMatrix = glm::scale(terrainModelMatrix, glm::vec3(100.0f, 100.0f, 100.0f));
	
	// Model matrix for the tree
	glm::mat4 treeModelMatrix = glm::mat4(1.0f);
	// Position the tree to be visible on the terrain
	treeModelMatrix = glm::translate(treeModelMatrix, glm::vec3(20.0f, -10.0f, 0.0f));
	// Scale the tree to be more visible
	treeModelMatrix = glm::scale(treeModelMatrix, glm::vec3(10.0f, 10.0f, 10.0f));

	lightShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModelMatrix));
	glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);

	// Position camera much higher and further back to view the larger terrain
	Camera camera(width, height, glm::vec3(0.0f, 20.0f, 50.0f));
	
	// Increase camera speed to move around the much larger terrain
	camera.speed = 10.0f;

	// Sun (light) animation parameters
	float sunRadius = 200.0f;
	float sunHeight = 1000.0f; // Increased from 5.0f to 10.0f
	float sunSpeed = 0.1f; // radians per second
	float sunAngle = 0.0f;

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// Specify the color of the background
		glClearColor(0.3f, 0.1f, 0.1f, 1.0f); // Dark red/brown to contrast with the terrain
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Handles camera inputs
		camera.Inputs(window);
		// Updates and exports the camera matrix to the Vertex Shader
		camera.updateMatrix(45.0f, 1.0f, 100000.0f);

		// Animate sun position (day/night cycle)
		float time = glfwGetTime();
		sunAngle = fmod(time * sunSpeed, 2.0f * M_PI);
		lightPos = glm::vec3(
			sunRadius * cos(sunAngle),
			sunHeight * sin(sunAngle),
			sunRadius * sin(sunAngle)
		);

		// Light color transitions: yellowish during day, bluish at night
		float intensity = glm::clamp(sin(sunAngle), 0.0f, 1.5f);
		glm::vec4 dayColor = glm::vec4(1.4f, 1.1f, 0.8f, 1.0f);
		glm::vec4 nightColor = glm::vec4(0.1f, 0.2f, 0.4f, 1.0f);
		lightColor = glm::mix(nightColor, dayColor, intensity);

		// Update light model matrix for the sphere marker
		lightModelMatrix = glm::mat4(1.0f);
		lightModelMatrix = glm::translate(lightModelMatrix, lightPos);
		lightModelMatrix = glm::scale(lightModelMatrix, glm::vec3(5.0f, 5.0f, 5.0f)); // Increased size


		// Draw the light marker (sphere)
		lightShader.Activate();
		glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModelMatrix));
		glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
		light.Draw(lightShader, camera);


		// Update uniforms for the main shader (lighting)
		shaderProgram.Activate();
		glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		
		// Default material properties for models
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "material.ambient"), 0.2f, 0.2f, 0.2f);
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "material.diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "material.specular"), 0.5f, 0.5f, 0.5f);
		glUniform1f(glGetUniformLocation(shaderProgram.ID, "material.shininess"), 32.0f);
		
		// Draw terrain
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(terrainModelMatrix));
		terrainModel.Draw(shaderProgram, camera);
		
		// Draw tree
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(treeModelMatrix));
		treeModel.Draw(shaderProgram, camera);

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