#include "model.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <string>

struct Path {
    std::string path;
    Path(const std::string& p) : path(p) {}
    Path(const char* p) : path(p) {}
    
    Path parent_path() const {
        size_t pos = path.find_last_of("/\\");
        if (pos != std::string::npos) {
            return Path(path.substr(0, pos));
        }
        return Path("");
    }
    
    Path operator/(const std::string& other) const {
        if (path.empty()) return Path(other);
        if (path.back() == '/' || path.back() == '\\') {
            return Path(path + other);
        }
        return Path(path + "/" + other);
    }
    
    std::string string() const {
        return path;
    }
};

namespace fs {
    using Path = ::Path;
}

Model::Model(const char* file,bool LoadCollider) {
    loadOBJ(file);
    if (LoadCollider) {
        buildCollider(glm::mat4(1.0f));
    }

}

Model::~Model() {}

void Model::Draw(Shader& shader, Camera& camera, const glm::mat4& modelMatrix) {
    for (auto& mesh : meshes) {
        // Set material properties for this mesh
        if (!mesh.materialName.empty() && materials.find(mesh.materialName) != materials.end()) {
            const Material& material = materials[mesh.materialName];
            
            // Set material uniforms in shader
            shader.Activate();
            glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
            glUniform1f(glGetUniformLocation(shader.ID, "textureTiling"), textureTiling);
            glUniform3fv(glGetUniformLocation(shader.ID, "material.ambient"), 1, glm::value_ptr(material.ambient));
            glUniform3fv(glGetUniformLocation(shader.ID, "material.diffuse"), 1, glm::value_ptr(material.diffuse));
            glUniform3fv(glGetUniformLocation(shader.ID, "material.specular"), 1, glm::value_ptr(material.specular));
            glUniform1f(glGetUniformLocation(shader.ID, "material.shininess"), material.shininess);
        }
        
        mesh.Draw(shader, camera);
    }
}

void Model::AddTexture(const Texture& texture)
{
    for (auto& mesh : meshes)
    {
        mesh.textures.clear();          // Vide les textures existantes
        mesh.textures.push_back(texture);  // Ajoute la nouvelle texture
    }
}



void Model::loadOBJ(const char* file) {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texUVs;
    std::map<std::string, std::vector<Vertex>> materialVertices;
    std::map<std::string, std::vector<GLuint>> materialIndices;
    std::map<std::string, GLuint> uniqueVertexMap;
    std::string currentMaterial = "default";

    std::ifstream in(file);
    if (!in) {
        std::cerr << "Cannot open OBJ file: " << file << std::endl;
        return;
    }
    
    std::cout << "Started loading OBJ file: " << file << std::endl;
    
    // Get the directory of the OBJ file
    fs::Path objPath(file);
    fs::Path objDir = objPath.parent_path();
    
    std::string mtlFile;
    
    std::string line;
    while (std::getline(in, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;
        if (prefix == "v") {
            glm::vec3 pos;
            iss >> pos.x >> pos.y >> pos.z;
            positions.push_back(pos);
        } else if (prefix == "vt") {
            glm::vec2 uv;
            iss >> uv.x >> uv.y;
            texUVs.push_back(uv);
        } else if (prefix == "vn") {
            glm::vec3 norm;
            iss >> norm.x >> norm.y >> norm.z;
            normals.push_back(norm);
        } else if (prefix == "mtllib") {
            iss >> mtlFile;
            std::cout << "MTL file referenced: " << mtlFile << std::endl;
        } else if (prefix == "usemtl") {
            iss >> currentMaterial;
            std::cout << "Using material: " << currentMaterial << std::endl;
            // Create entries for the material if they don't exist yet
            if (materialVertices.find(currentMaterial) == materialVertices.end()) {
                materialVertices[currentMaterial] = std::vector<Vertex>();
                materialIndices[currentMaterial] = std::vector<GLuint>();
                
                // Initialize material with default values if it doesn't exist
                if (materials.find(currentMaterial) == materials.end()) {
                    materials[currentMaterial] = {
                        glm::vec3(1.0f), // default ambient
                        glm::vec3(1.0f), // default diffuse
                        glm::vec3(0.0f), // default specular
                        16.0f,           // default shininess
                        std::vector<Texture>() // empty textures
                    };
                }
            }
        } else if (prefix == "f") {
            std::string vertStr;
            std::vector<GLuint> faceIndices;
            while (iss >> vertStr) {
                // Create a unique key for the vertex based on material+vertexdata
                std::string uniqueKey = currentMaterial + ":" + vertStr;
                
                if (uniqueVertexMap.count(uniqueKey) == 0) {
                    std::istringstream viss(vertStr);
                    std::string vStr, tStr, nStr;
                    std::getline(viss, vStr, '/');
                    std::getline(viss, tStr, '/');
                    std::getline(viss, nStr, '/');
                    int vIdx = std::stoi(vStr) - 1;
                    int tIdx = tStr.empty() ? -1 : (std::stoi(tStr) - 1);
                    int nIdx = nStr.empty() ? -1 : (std::stoi(nStr) - 1);

                    Vertex vert;
                    vert.position = positions[vIdx];
                    vert.color = glm::vec3(1.0f); // default color
                    vert.textUV = tIdx >= 0 ? texUVs[tIdx] : glm::vec2(0.0f);
                    vert.normal = nIdx >= 0 ? normals[nIdx] : glm::vec3(0.0f, 1.0f, 0.0f);
                    materialVertices[currentMaterial].push_back(vert);
                    GLuint idx = static_cast<GLuint>(materialVertices[currentMaterial].size() - 1);
                    uniqueVertexMap[uniqueKey] = idx;
                    faceIndices.push_back(idx);
                } else {
                    faceIndices.push_back(uniqueVertexMap[uniqueKey]);
                }
            }
            // Triangulate face (fan method)
            for (size_t i = 1; i + 1 < faceIndices.size(); ++i) {
                materialIndices[currentMaterial].push_back(faceIndices[0]);
                materialIndices[currentMaterial].push_back(faceIndices[i]);
                materialIndices[currentMaterial].push_back(faceIndices[i + 1]);
            }
        }
    }
    
    // Now load the MTL file if specified
    if (!mtlFile.empty()) {
        loadMTL((objDir / mtlFile).string().c_str(), materials);
    }
    
    // Create a mesh for each material
    // Using a non-C++17 loop instead of structured binding
    for (auto it = materialVertices.begin(); it != materialVertices.end(); ++it) {
        const std::string& material = it->first;
        const std::vector<Vertex>& vertices = it->second;
        
        if (!vertices.empty() && !materialIndices[material].empty()) {
            std::cout << "Creating mesh for material: " << material << " with " 
                      << vertices.size() << " vertices and " 
                      << materialIndices[material].size() << " indices" << std::endl;
            
            // Create a new mesh with material textures
            Mesh mesh(materialVertices[material], materialIndices[material], materials[material].textures);
            
            // Store material name with the mesh for later use in Draw
            mesh.materialName = material;
            
            meshes.push_back(mesh);
        }
    }
    
    std::cout << "Finished loading OBJ with " << positions.size() << " vertices, " 
              << normals.size() << " normals, " << texUVs.size() << " texture coordinates, "
              << "and " << meshes.size() << " meshes" << std::endl;
}

void Model::loadMTL(const char* file, std::map<std::string, Material>& materials) {
    std::ifstream in(file);
    if (!in) {
        std::cerr << "Cannot open MTL file: " << file << std::endl;
        return;
    }
    
    std::cout << "Started loading MTL file: " << file << std::endl;
    
    // Get the directory of the MTL file
    fs::Path mtlPath(file);
    fs::Path mtlDir = mtlPath.parent_path();
    
    std::string currentMaterial;
    std::string line;
    while (std::getline(in, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;
        
        if (prefix == "newmtl") {
            iss >> currentMaterial;
            std::cout << "Found material in MTL: " << currentMaterial << std::endl;
            
            // Initialize material with default values
            if (materials.find(currentMaterial) == materials.end()) {
                materials[currentMaterial] = {
                    glm::vec3(1.0f), // default ambient
                    glm::vec3(1.0f), // default diffuse
                    glm::vec3(0.0f), // default specular
                    16.0f,           // default shininess
                    std::vector<Texture>() // empty textures
                };
            }
        } else if (prefix == "Ka" && !currentMaterial.empty()) {
            float r, g, b;
            iss >> r >> g >> b;
            materials[currentMaterial].ambient = glm::vec3(r, g, b);
        } else if (prefix == "Kd" && !currentMaterial.empty()) {
            float r, g, b;
            iss >> r >> g >> b;
            materials[currentMaterial].diffuse = glm::vec3(r, g, b);
        } else if (prefix == "Ks" && !currentMaterial.empty()) {
            float r, g, b;
            iss >> r >> g >> b;
            materials[currentMaterial].specular = glm::vec3(r, g, b);
        } else if (prefix == "Ns" && !currentMaterial.empty()) {
            float shininess;
            iss >> shininess;
            materials[currentMaterial].shininess = shininess;
        } else if (prefix == "map_Kd" && !currentMaterial.empty()) {
            std::string texturePath;
            iss >> texturePath;
            
            // Construct full path to texture
            fs::Path fullTexturePath = mtlDir / texturePath;
            
            // Use auto-detection for texture format (pass 0 for format)
            Texture diffuseTex(fullTexturePath.string().c_str(), "diffuse", GL_TEXTURE0);
            materials[currentMaterial].textures.push_back(diffuseTex);
        }
    }
    
    // Print summary in a single line instead of full details
    std::cout << "Loaded " << materials.size() << " materials" << std::endl;
}

void Model::buildCollider(const glm::mat4& modelMatrix) {
    if (meshes.empty()) return;

    glm::vec3 min = meshes[0].getMinVertex();
    glm::vec3 max = meshes[0].getMaxVertex();

    for (size_t i = 1; i < meshes.size(); ++i) {
        glm::vec3 meshMin = meshes[i].getMinVertex();
        glm::vec3 meshMax = meshes[i].getMaxVertex();
        min = glm::min(min, meshMin);
        max = glm::max(max, meshMax);
    }

    // Appliquer la transformation du modèle au collider
    glm::vec3 corners[8] = {
        glm::vec3(min.x, min.y, min.z),
        glm::vec3(min.x, min.y, max.z),
        glm::vec3(min.x, max.y, min.z),
        glm::vec3(min.x, max.y, max.z),
        glm::vec3(max.x, min.y, min.z),
        glm::vec3(max.x, min.y, max.z),
        glm::vec3(max.x, max.y, min.z),
        glm::vec3(max.x, max.y, max.z)
    };
    glm::vec3 worldMin( std::numeric_limits<float>::max());
    glm::vec3 worldMax(-std::numeric_limits<float>::max());
    for (int i = 0; i < 8; ++i) {
        glm::vec4 transformed = modelMatrix * glm::vec4(corners[i], 1.0f);
        worldMin = glm::min(worldMin, glm::vec3(transformed));
        worldMax = glm::max(worldMax, glm::vec3(transformed));
    }
    collider = Collider(worldMin, worldMax);

    // Debug print
    std::cout << "Collider built: min(" << worldMin.x << "," << worldMin.y << "," << worldMin.z
              << ") max(" << worldMax.x << "," << worldMax.y << "," << worldMax.z << ")" << std::endl;
}

// Add implementation for buildComponentColliders
void Model::buildComponentColliders(const glm::mat4& modelMatrix) {
    componentColliders.clear();
    
    // First create a collider for each mesh/material separately
    for (auto& mesh : meshes) {
        if (mesh.materialName.empty()) continue;
        
        // Get min/max vertices for this mesh only
        glm::vec3 meshMin = mesh.getMinVertex();
        glm::vec3 meshMax = mesh.getMaxVertex();
        
        // Transform them by the model matrix
        glm::vec3 corners[8] = {
            glm::vec3(meshMin.x, meshMin.y, meshMin.z),
            glm::vec3(meshMin.x, meshMin.y, meshMax.z),
            glm::vec3(meshMin.x, meshMax.y, meshMin.z),
            glm::vec3(meshMin.x, meshMax.y, meshMax.z),
            glm::vec3(meshMax.x, meshMin.y, meshMin.z),
            glm::vec3(meshMax.x, meshMin.y, meshMax.z),
            glm::vec3(meshMax.x, meshMax.y, meshMin.z),
            glm::vec3(meshMax.x, meshMax.y, meshMax.z)
        };
        
        glm::vec3 worldMin(std::numeric_limits<float>::max());
        glm::vec3 worldMax(-std::numeric_limits<float>::max());
        
        for (int i = 0; i < 8; ++i) {
            glm::vec4 transformed = modelMatrix * glm::vec4(corners[i], 1.0f);
            worldMin = glm::min(worldMin, glm::vec3(transformed));
            worldMax = glm::max(worldMax, glm::vec3(transformed));
        }
        
        // Create a collider for this specific material/component
        componentColliders[mesh.materialName] = Collider(worldMin, worldMax);
        
        // Debug output
        std::cout << "Component collider for " << mesh.materialName << ": "
                  << "min(" << worldMin.x << "," << worldMin.y << "," << worldMin.z << ") "
                  << "max(" << worldMax.x << "," << worldMax.y << "," << worldMax.z << ")" << std::endl;
    }
}

// Get a specific component collider by material name
Collider* Model::getComponentCollider(const std::string& materialName) {
    auto it = componentColliders.find(materialName);
    if (it != componentColliders.end()) {
        return &(it->second);
    }
    return nullptr;
}

// Get all component colliders
std::vector<Collider> Model::getAllComponentColliders() const {
    std::vector<Collider> result;
    for (const auto& pair : componentColliders) {
        result.push_back(pair.second);
    }
    return result;
}
