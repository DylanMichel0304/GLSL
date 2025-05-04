#ifndef CUBEMAPS_H
#define CUBEMAPS_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include <iostream>

#include "shaderClass.h"
#include "Camera.h"

class Cubemaps {
public:
    Cubemaps(const std::string facesCubemap[6], const char* vertexPath, const char* fragmentPath);
    void Draw(Camera& camera, int screenWidth, int screenHeight);
    void Delete();

private:
    unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
    unsigned int cubemapTexture;
    Shader skyboxShader;

    void setup();
    void loadCubemap(const std::string facesCubemap[6]);

    // Keep vertices and indices data accessible within the class
    static const float skyboxVertices[];
    static const unsigned int skyboxIndices[];
};

#endif // CUBEMAPS_H
