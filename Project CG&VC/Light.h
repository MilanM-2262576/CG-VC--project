#pragma once

#include <glm/glm.hpp>
#include "Shader.h"

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
    Shader lightShader;
};