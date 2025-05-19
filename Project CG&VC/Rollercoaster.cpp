#include "RollerCoaster.h"
#include "BezierCurve.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

// Constructor
RollerCoaster::RollerCoaster(std::vector<std::vector<glm::vec3>> bezierSegments, int cylinderSegments) 
    : m_cylinderSegments(cylinderSegments), m_shader(".\\BezierShader.vert", ".\\BezierShader.frag") {

    for (const auto& segment : bezierSegments) {
        m_curves.emplace_back(segment);
    }

    // Genereer cilinders voor de Bézier-curves
    GenerateCylinders();
    GeneratePillars(10.0f); // Elke 10 eenheden een pilaar
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
    m_cylinderVAOs.clear();

    for (auto& curve : m_curves) {
        std::vector<glm::vec3> curvePoints = curve.GeneratePoints(100);
        for (size_t i = 0; i < curvePoints.size() - 1; ++i) {
            glm::vec3 start = curvePoints[i];
            glm::vec3 end = curvePoints[i + 1];
            glm::vec3 direction = glm::normalize(end - start);

            // Bepaal de lokale up en right vector
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
            if (glm::length(glm::cross(direction, up)) < 0.01f)
                up = glm::vec3(1.0f, 0.0f, 0.0f);
            glm::vec3 right = glm::normalize(glm::cross(up, direction));
            up = glm::normalize(glm::cross(direction, right));

            // Offset voor linker- en rechterrail
            float halfWidth = m_trackWidth * 0.5f;
            glm::vec3 leftOffset = -right * halfWidth;
            glm::vec3 rightOffset = right * halfWidth;

            // Genereer VAO voor linkerrail
            unsigned int leftVAO = createCylinderVAO(
                start + leftOffset,
                end + leftOffset,
                m_cylinderRadius,
                m_cylinderSegments
            );
            m_cylinderVAOs.push_back(leftVAO);

            // Genereer VAO voor rechterrail
            unsigned int rightVAO = createCylinderVAO(
                start + rightOffset,
                end + rightOffset,
                m_cylinderRadius,
                m_cylinderSegments
            );
            m_cylinderVAOs.push_back(rightVAO);

            // Genereer VAO voor dwarsligger (crossbar)
            unsigned int crossbarVAO = createCrossbarVAO(
                start + leftOffset,
                start + rightOffset,
                m_crossbarThickness // e.g. 0.05f
            );
            m_cylinderVAOs.push_back(crossbarVAO);
        }
    }
}

// Hulpmethod om pillars te generaten
void RollerCoaster::GeneratePillars(float interval) {
    for (auto& curve : m_curves) {
        std::vector<glm::vec3> curvePoints = curve.GeneratePoints(32);
        float accumulated = 0.0f;
        glm::vec3 last = curvePoints[0];
        for (size_t i = 1; i < curvePoints.size(); ++i) {
            float segmentLength = glm::length(curvePoints[i] - last);
            accumulated += segmentLength;
            if (accumulated >= interval) {
                glm::vec3 top = curvePoints[i];
                glm::vec3 bottom = glm::vec3(top.x, 0.0f, top.z); // Naar de grond
                unsigned int pillarVAO = createCylinderVAO(
                    bottom,
                    top,
                    0.2f, // Stel deze in als een kleine waarde, bv. 0.2f
                    m_cylinderSegments
                );
                m_cylinderVAOs.push_back(pillarVAO);
                accumulated = 0.0f;
            }
            last = curvePoints[i];
        }
    }
}

// Render-methode
void RollerCoaster::Render(const glm::mat4& projection, const glm::mat4& view) {
    m_shader.use();
    m_shader.setMat4("projection", projection);
    m_shader.setMat4("view", view);

    glm::vec3 railColor(0.7f, 0.7f, 0.7f);
    glm::vec3 crossbarColor(0.2f, 0.2f, 0.2f);
    glm::vec3 pillarColor(0.4f, 0.4f, 0.4f);

    // Bepaal het aantal VAOs voor rails en crossbars (3 per segment)
    size_t nonPillarCount = 0;
    for (auto& curve : m_curves) {
        std::vector<glm::vec3> curvePoints = curve.GeneratePoints(100);
        nonPillarCount += (curvePoints.size() - 1) * 3;
    }

    for (size_t i = 0; i < m_cylinderVAOs.size(); ++i) {
        glm::mat4 model = glm::mat4(1.0f);
        m_shader.setMat4("model", model);

        if (i < nonPillarCount) {
            // Elke derde VAO is een crossbar
            if ((i % 3) == 2) {
                m_shader.setVec3("objectColor", crossbarColor);
            }
            else {
                m_shader.setVec3("objectColor", railColor);
            }
        }
        else {
            // Dit zijn de pilaren: altijd grijs
            m_shader.setVec3("objectColor", pillarColor);
        }

        glBindVertexArray(m_cylinderVAOs[i]);
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

unsigned int RollerCoaster::createCrossbarVAO(const glm::vec3& left, const glm::vec3& right, float thickness) {
    glm::vec3 dir = glm::normalize(right - left);
    glm::vec3 up = glm::vec3(0, 1, 0);
    if (glm::abs(glm::dot(dir, up)) > 0.99f) up = glm::vec3(1, 0, 0);
    glm::vec3 normal = glm::normalize(glm::cross(dir, up));
    up = glm::normalize(glm::cross(normal, dir));
    float halfThick = thickness * 0.5f;

    // 8 corners of a box
    std::vector<glm::vec3> corners = {
        left + up * halfThick + normal * halfThick,
        left + up * halfThick - normal * halfThick,
        left - up * halfThick - normal * halfThick,
        left - up * halfThick + normal * halfThick,
        right + up * halfThick + normal * halfThick,
        right + up * halfThick - normal * halfThick,
        right - up * halfThick - normal * halfThick,
        right - up * halfThick + normal * halfThick
    };

    std::vector<float> vertices;
    for (const auto& v : corners) {
        vertices.push_back(v.x);
        vertices.push_back(v.y);
        vertices.push_back(v.z);
    }

    std::vector<unsigned int> indices = {
        0,1,2, 2,3,0, // left face
        4,5,6, 6,7,4, // right face
        0,4,7, 7,3,0, // top face
        1,5,6, 6,2,1, // bottom face
        0,1,5, 5,4,0, // front face
        3,2,6, 6,7,3  // back face
    };

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

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