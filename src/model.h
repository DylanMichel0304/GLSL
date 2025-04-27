#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include <vector>
#include <string>
#include <map>
#include "Mesh.h"
#include "shaderClass.h"
#include "Camera.h"

// Structure to hold material properties from MTL files
struct Material {
    glm::vec3 ambient;  // Ka
    glm::vec3 diffuse;  // Kd
    glm::vec3 specular; // Ks
    float shininess;    // Ns
    std::vector<Texture> textures;
};

class Model
{
public:
    // Loads model from file
    Model(const char* file);
    // Draws the model using the given shader and camera
    void Draw(Shader& shader, Camera& camera);
    // Check if model has loaded meshes
    bool IsLoaded() const { return !meshes.empty(); }
    // Destructor
    ~Model();

private:
    std::vector<Mesh> meshes;
    std::map<std::string, Material> materials;
    
    // Loads an OBJ file
    void loadOBJ(const char* file);
    // Loads an MTL file and associated textures
    void loadMTL(const char* file, std::map<std::string, Material>& materials);
};

#endif