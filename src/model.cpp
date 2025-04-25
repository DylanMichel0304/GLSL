#include "model.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>

Model::Model(const char* file) {
    loadOBJ(file);
}

Model::~Model() {}

void Model::Draw(Shader& shader, Camera& camera) {
    for (auto& mesh : meshes) {
        mesh.Draw(shader, camera);
    }
}

void Model::loadOBJ(const char* file) {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texUVs;
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::map<std::string, GLuint> uniqueVertexMap;

    std::ifstream in(file);
    if (!in) {
        std::cerr << "Cannot open OBJ file: " << file << std::endl;
        return;
    }
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
        } else if (prefix == "f") {
            std::string vertStr;
            std::vector<GLuint> faceIndices;
            while (iss >> vertStr) {
                if (uniqueVertexMap.count(vertStr) == 0) {
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
                    vertices.push_back(vert);
                    GLuint idx = static_cast<GLuint>(vertices.size() - 1);
                    uniqueVertexMap[vertStr] = idx;
                    faceIndices.push_back(idx);
                } else {
                    faceIndices.push_back(uniqueVertexMap[vertStr]);
                }
            }
            // Triangulate face (fan method)
            for (size_t i = 1; i + 1 < faceIndices.size(); ++i) {
                indices.push_back(faceIndices[0]);
                indices.push_back(faceIndices[i]);
                indices.push_back(faceIndices[i + 1]);
            }
        }
    }
    std::vector<Texture> textures; // You can load textures as needed
    meshes.push_back(Mesh(vertices, indices, textures));
}