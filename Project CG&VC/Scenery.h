#pragma once

#include <glm/glm.hpp>
#include "Model.h"
#include "Shader.h"

class Scenery {
public:
    Scenery(const std::string& modelPath, const glm::vec3& position, float scale = 1.0f)
        : m_model(modelPath), m_position(position), m_scale(scale), m_shader(".\\SceneryShader.vert", ".\\SceneryShader.frag") {
    }

    virtual ~Scenery() = default;

    virtual void Render(const glm::mat4& projection, const glm::mat4& view) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, m_position);
        model = glm::rotate(model, m_rotation.y, glm::vec3(0, 1, 0)); // Y-as
        model = glm::scale(model, glm::vec3(m_scale));

        m_shader.use();
        m_shader.setMat4("projection", projection);
        m_shader.setMat4("view", view);
        m_shader.setMat4("model", model);
        m_model.Draw(m_shader);
    }

    const glm::vec3& GetPosition() const { return m_position; }
    void SetPosition(const glm::vec3& pos) { m_position = pos; }
    float GetScale() const { return m_scale; }
    void SetScale(float scale) { m_scale = scale; }
    void SetRotation(const glm::vec3& rot) { m_rotation = rot; }
    const glm::vec3& GetRotation() const { return m_rotation; }

protected:
    Model m_model;
    Shader m_shader;
    glm::vec3 m_position;
    float m_scale;
    glm::vec3 m_rotation = glm::vec3(0.0f); 

};
