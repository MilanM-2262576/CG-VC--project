#include "Light.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

float cubeVertices[] = {
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f
};

Light::Light(const glm::vec3& position, std::string modelPath, const glm::vec3& color)
    : position(position), m_model(modelPath), color(color), initialPosition(position), lightShader(".\\LightSourceShader.vert", ".\\LightSourceShader.frag") {
}

// In case we want to move the lights
void Light::Update(float time) {
    // Move the light up and down using a sine wave
    float amplitude = 0.5f; // How far up and down to move
    float frequency = 1.0f; // How fast to move
    position.y = initialPosition.y + amplitude * sin(frequency * time);
}

void Light::Render(const glm::mat4& projection, const glm::mat4& view) {
    lightShader.use();
    lightShader.setVec3("lightColor", color);
    lightShader.setMat4("projection", projection);
    lightShader.setMat4("view", view);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(0.8f));
    lightShader.setMat4("model", model);

    m_model.Draw(lightShader);
}
