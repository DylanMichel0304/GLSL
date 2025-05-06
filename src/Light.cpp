#include "Light.h"
#include <glm/gtc/type_ptr.hpp>

Light::Light(int type, const glm::vec3& pos, const glm::vec3& dir, const glm::vec4& color, Model* mesh)
    : type(type), position(pos), direction(dir), color(color), mesh(mesh) {}

void Light::sendToShader(const Shader& shader, int index) const {
    std::string number = std::to_string(index);
    glUniform3fv(glGetUniformLocation(shader.ID, ("lights[" + number + "].position").c_str()), 1, glm::value_ptr(position));
    glUniform3fv(glGetUniformLocation(shader.ID, ("lights[" + number + "].direction").c_str()), 1, glm::value_ptr(direction));
    glUniform4fv(glGetUniformLocation(shader.ID, ("lights[" + number + "].color").c_str()), 1, glm::value_ptr(color));
    glUniform1i(glGetUniformLocation(shader.ID, ("lights[" + number + "].type").c_str()), type);
}

void Light::drawMesh( Shader& shader, Camera& camera,const glm::mat4& modelMatrix) const {
    if (mesh) {
        shader.Activate();
        mesh->Draw(shader, camera, modelMatrix);
    }
}
