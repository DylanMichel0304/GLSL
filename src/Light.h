#pragma once
#include <glm/glm.hpp>
#include <string>
#include "model.h"
#include "shaderClass.h"

class Light {
public:
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec4 color;
    int type; // 0 = Directional, 1 = Point, 2 = Spot

    Model* mesh; // Mesh associé à la lumière (peut être nullptr)

    Light(int type, const glm::vec3& pos, const glm::vec3& dir, const glm::vec4& color, Model* mesh = nullptr);

    void sendToShader(const Shader& shader, int index) const;
    void drawMesh( Shader& shader,Camera& camera, const glm::mat4& modelMatrix) const;
};
