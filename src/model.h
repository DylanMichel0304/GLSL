#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include <vector>
#include <string>
#include "Mesh.h"
#include "shaderClass.h"
#include "Camera.h"

class Model
{
public:
    // Loads model from file
    Model(const char* file);
    // Draws the model using the given shader and camera
    void Draw(Shader& shader, Camera& camera);
    // Destructor
    ~Model();

private:
    std::vector<Mesh> meshes;
    // Loads an OBJ file
    void loadOBJ(const char* file);
};

#endif