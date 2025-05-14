#include "RollerCoaster.h"
#include "BezierCurve.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

// Constructor
RollerCoaster::RollerCoaster(std::vector<std::vector<glm::vec3>> bezierSegments, float cylinderRadius, int cylinderSegments) : m_cylinderRadius(cylinderRadius), m_cylinderSegments(cylinderSegments), m_shader(".\\BezierShader.vert", ".\\BezierShader.frag") {

    for (const auto& segment : bezierSegments) {
        m_curves.emplace_back(segment);
    }

    // Genereer cilinders voor de Bézier-curves
    GenerateCylinders();
}

// Destructor
RollerCoaster::~RollerCoaster() {
    // Verwijder alle VAOs
    for (unsigned int vao : m_cylinderVAOs) {
        glDeleteVertexArrays(1, &vao);
    }
}

// Hulpmethode om cilinders te genereren
void RollerCoaster::GenerateCylinders() {
    for (auto& curve : m_curves) {
        std::vector<glm::vec3> curvePoints = curve.GeneratePoints(100); // 100 punten per curve
        for (size_t i = 0; i < curvePoints.size() - 1; ++i) {
            unsigned int cylinderVAO = createCylinderVAO(
                curvePoints[i],       // Startpunt van het segment
                curvePoints[i + 1],   // Eindpunt van het segment
                m_cylinderRadius,     // Radius van de cilinder
                m_cylinderSegments    // Aantal segmenten voor de cirkel
            );
            m_cylinderVAOs.push_back(cylinderVAO);
        }
    }
}

// Render-methode
void RollerCoaster::Render(const glm::mat4& projection, const glm::mat4& view) {
    m_shader.use();
    m_shader.setMat4("projection", projection);
    m_shader.setMat4("view", view);

    for (unsigned int vao : m_cylinderVAOs) {
        glm::mat4 model = glm::mat4(1.0f);
        m_shader.setMat4("model", model);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, m_cylinderSegments * 6, GL_UNSIGNED_INT, 0);
    }
}

unsigned int RollerCoaster::createCylinderVAO(const glm::vec3& start, const glm::vec3& end, float radius, int segments) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    glm::vec3 direction = glm::normalize(end - start);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    if (glm::length(glm::cross(direction, up)) < 0.01f) {
        up = glm::vec3(1.0f, 0.0f, 0.0f);
    }
    glm::vec3 right = glm::normalize(glm::cross(up, direction));
    up = glm::normalize(glm::cross(direction, right));

    // Generate vertices for the cylinder
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * glm::pi<float>() * i / segments;
        float x = radius * cos(angle);
        float y = radius * sin(angle);

        glm::vec3 offset = right * x + up * y;

        // Bottom circle
        glm::vec3 bottomVertex = start + offset;
        vertices.push_back(bottomVertex.x);
        vertices.push_back(bottomVertex.y);
        vertices.push_back(bottomVertex.z);

        // Top circle
        glm::vec3 topVertex = end + offset;
        vertices.push_back(topVertex.x);
        vertices.push_back(topVertex.y);
        vertices.push_back(topVertex.z);
    }

    // Generate indices for the cylinder
    for (int i = 0; i < segments; ++i) {
        int bottom1 = i * 2;
        int top1 = bottom1 + 1;
        int bottom2 = (i + 1) * 2;
        int top2 = bottom2 + 1;

        // First triangle
        indices.push_back(bottom1);
        indices.push_back(top1);
        indices.push_back(bottom2);

        // Second triangle
        indices.push_back(top1);
        indices.push_back(top2);
        indices.push_back(bottom2);
    }

    // Create VAO, VBO, and EBO
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}

// Clean up method
void RollerCoaster::CleanUp() {
    for (unsigned int vao : m_cylinderVAOs) {
        glDeleteVertexArrays(1, &vao);
    }
    m_cylinderVAOs.clear(); 
}