#pragma once

#include <glm/glm.hpp>
#include "Shader.h"

struct PointLight {
    glm::vec3 position;
    glm::vec3 color;
    float constant;
    float linear;
    float quadratic;
};

class Light {
public:
    glm::vec3 position;
    glm::vec3 color;

    Light(const glm::vec3& position, const glm::vec3& color);
    ~Light();

    void Initialize();
    void Update(float time);
    void Render(const glm::mat4& projection, const glm::mat4& view);

private:
    unsigned int VAO, VBO;
    glm::vec3 initialPosition;
    Shader lightShader;
};