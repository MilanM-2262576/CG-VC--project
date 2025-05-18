#pragma once

#include <glm/glm.hpp>
#include <iostream>

#include "Shader.h"
#include "Model.h"

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

    Light(const glm::vec3& position, std::string modelPath, const glm::vec3& color);

    void Update(float time);
    void Render(const glm::mat4& projection, const glm::mat4& view);

private:
    Shader lightShader;
    Model m_model;

    glm::vec3 initialPosition;
    
};