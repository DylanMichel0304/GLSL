#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include <vector>
#include <string>
#include <map>
#include "Mesh.h"
#include "shaderClass.h"
#include "Camera.h"
#include "Collider.h"

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
    Model(const char* file, bool LoadCollider = true);
    // Draws the model using the given shader and camera
    void Draw(Shader& shader, Camera& camera, const glm::mat4& modelMatrix);
    // Check if model has loaded meshes
    bool IsLoaded() const { return !meshes.empty(); }
    // Destructor
    ~Model();

    void buildCollider(const glm::mat4& modelMatrix);
    
    // Build separate colliders for each material/component
    void buildComponentColliders(const glm::mat4& modelMatrix);
    
    // Get a specific component collider by material name
    Collider* getComponentCollider(const std::string& materialName);
    
    // Get all component colliders
    std::vector<Collider> getAllComponentColliders() const;
    
    void AddTexture(const Texture& texture);

    // Set the texture tiling factor for this model
    void SetTextureTiling(float tiling) { textureTiling = tiling; }
    // Get the current texture tiling factor
    float GetTextureTiling() const { return textureTiling; }

    Collider collider;  // Main collider (whole model)
    std::vector<Mesh> meshes;

private:
    // Default texture tiling factor (50.0f is the original value)
    float textureTiling = 50.0f;
    
    std::map<std::string, Material> materials;
    std::map<std::string, Collider> componentColliders;  // Colliders for individual components
    
    // Loads an OBJ file
    void loadOBJ(const char* file);
    // Loads an MTL file and associated textures
    void loadMTL(const char* file, std::map<std::string, Material>& materials);
};

#endif