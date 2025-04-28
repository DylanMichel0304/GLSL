#include"src/Mesh.h"
#include"src/model.h"
#include"src/Cubemaps.h"
#include<iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


const unsigned int width = 1920;
const unsigned int height = 1080;

// Vertices coordinates
Vertex vertices[] =
{ //               COORDINATES           /            COLORS          /           NORMALS         /       TEXTURE COORDINATES    //
	Vertex{glm::vec3(-1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
	Vertex{glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
	Vertex{glm::vec3( 1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
	Vertex{glm::vec3( 1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)}
};

// Indices for vertices order
GLuint indices[] =
{
	0, 1, 2,
	0, 2, 3
};

Vertex lightVertices[] =
{ //     COORDINATES     //
	Vertex{glm::vec3(-0.1f, -0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f,  0.1f)}
};

GLuint lightIndices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 4, 7,
	0, 7, 3,
	3, 7, 6,
	3, 6, 2,
	2, 6, 5,
	2, 5, 1,
	1, 5, 4,
	1, 4, 0,
	4, 5, 6,
	4, 6, 7
};

struct Light {
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec4 color;
    int type; // 0 = Directional, 1 = Point, 2 = Spot
};


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
		Texture((texPath + "planks.png").c_str(), "diffuse", 0, GL_RGBA, GL_UNSIGNED_BYTE),
		Texture((texPath + "planksSpec.png").c_str(), "specular", 1, GL_RED, GL_UNSIGNED_BYTE)
	};

	std::vector<Light> sceneLights;

	Light sun;
	sun.type = 0; // Directional
	sun.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
	sun.color = glm::vec4(1.0f, 1.0f, 0.9f, 1.0f); // Soleil jaune
	sceneLights.push_back(sun);

	Light lamp;
	lamp.type = 2; // Point light
	lamp.position = glm::vec3(0.0f, 0.5f, 0.0f);
	lamp.direction = glm::vec3(0.0f, -1.0f, 0.0f);
	lamp.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // Lampe rouge
	sceneLights.push_back(lamp);


	std::string facesCubemap[6] =
	{
		"assets/cubesmaps/right.jpg",
		"assets/cubesmaps/left.jpg",
		"assets/cubesmaps/top.jpg",
		"assets/cubesmaps/bottom.jpg",
		"assets/cubesmaps/front.jpg",
		"assets/cubesmaps/back.jpg"
	};

	// Generates Shader object using shaders default.vert and default.frag
	Shader shaderProgram("shader/default.vert", "shader/default.frag");
	// Store mesh data in vectors for the mesh
	std::vector <Vertex> verts(vertices, vertices + sizeof(vertices) / sizeof(Vertex));
	std::vector <GLuint> ind(indices, indices + sizeof(indices) / sizeof(GLuint));
	std::vector <Texture> tex(textures, textures + sizeof(textures) / sizeof(Texture));
	// Create floor mesh
	Mesh floor(verts, ind, tex);


	// Shader for light cube
	Shader lightShader("shader/light.vert", "shader/light.frag");
	// Store mesh data in vectors for the mesh
	std::vector <Vertex> lightVerts(lightVertices, lightVertices + sizeof(lightVertices) / sizeof(Vertex));
	std::vector <GLuint> lightInd(lightIndices, lightIndices + sizeof(lightIndices) / sizeof(GLuint));
	// Create light mesh
	Mesh light(lightVerts, lightInd, tex);


	// Instantiate the Cubemaps class
	Cubemaps skybox(facesCubemap, "shader/skybox.vert", "shader/skybox.frag");


	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.0f, 0.2f, 0.0f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	glm::vec3 objectPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::mat4 objectModel = glm::mat4(1.0f);
	objectModel = glm::translate(objectModel, objectPos);


	lightShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
	glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	shaderProgram.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(objectModel));
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);



	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Creates camera object
	Camera camera(width, height, glm::vec3(0.0f, 1.0f, 2.0f));


	float sunStrength = 1.0f;
	float fadeSpeed = 0.1f; // plus c'est grand, plus ça descend vite


	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Handles camera inputs
		camera.Inputs(window);
		// Updates and exports the camera matrix to the Vertex Shader
		camera.updateMatrix(45.0f, 0.1f, 100.0f); // Note: Adjusted near/far planes if needed

		// Activate and configure main shader program
		shaderProgram.Activate();

		// Envoie le nombre de lumières
		glUniform1i(glGetUniformLocation(shaderProgram.ID, "lightCount"), sceneLights.size());

		// Diminue la force du soleil progressivement
		if (sunStrength > 0.0f)
		sunStrength -= fadeSpeed * glfwGetTime();
		sunStrength = glm::max(sunStrength, 0.0f); // éviter qu'il passe en négatif

		// Met à jour la couleur du soleil
		sceneLights[0].color = glm::vec4(1.0f, 1.0f, 0.9f, 1.0f) * sunStrength;

		// Reset glfw time pour éviter qu'il parte trop vite
		glfwSetTime(0);


		// Envoie chaque lumière
		for (int i = 0; i < sceneLights.size(); i++) {
			std::string number = std::to_string(i);
			glUniform3fv(glGetUniformLocation(shaderProgram.ID, ("lights[" + number + "].position").c_str()), 1, glm::value_ptr(sceneLights[i].position));
			glUniform3fv(glGetUniformLocation(shaderProgram.ID, ("lights[" + number + "].direction").c_str()), 1, glm::value_ptr(sceneLights[i].direction));
			glUniform4fv(glGetUniformLocation(shaderProgram.ID, ("lights[" + number + "].color").c_str()), 1, glm::value_ptr(sceneLights[i].color));
			glUniform1i(glGetUniformLocation(shaderProgram.ID, ("lights[" + number + "].type").c_str()), sceneLights[i].type);
			
		}

		// Caméra position
		glUniform3fv(glGetUniformLocation(shaderProgram.ID, "camPos"), 1, glm::value_ptr(camera.Position));
        // Pass camera matrix to the main shader (replace view/proj lines)
        camera.Matrix(shaderProgram, "camMatrix");


		// Draws different meshes
		floor.Draw(shaderProgram, camera);
		light.Draw(lightShader, camera); // Make sure light.Draw also uses camera.view and camera.projection

        // Draw the skybox using the Cubemaps object
        skybox.Draw(camera, width, height);


        glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}



	// Delete all the objects we've created
	shaderProgram.Delete();
	lightShader.Delete();
    skybox.Delete(); // Delete skybox resources

	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}
