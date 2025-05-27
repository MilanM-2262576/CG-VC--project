#include "ParticleSystem.h"
#include "stb_image.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <random>

ParticleSystem::ParticleSystem(unsigned int maxParticles, const char* texturePath)
    : m_maxParticles(maxParticles), m_active(false), m_shader(".\\Particle.vert", ".\\Particle.frag")
{
    m_particles.resize(maxParticles);

    // Quad (billboard)
    float quadVertices[] = {
        // positions   // texcoords
        -0.5f, -0.5f, 0.0f, 0.0f,
         0.5f, -0.5f, 1.0f, 0.0f,
         0.5f,  0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f, 1.0f
    };
    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // pos
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1); // tex
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);

    m_texture = loadTexture(texturePath);

    // Init all particles as dead
    for (auto& p : m_particles) p.life = -1.0f;
}

ParticleSystem::~ParticleSystem() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteTextures(1, &m_texture);
}

void ParticleSystem::Update(float dt, const glm::vec3& emitterPos) {
    if (!m_active) return;
    for (auto& p : m_particles) {
        if (p.life > 0.0f) {
            p.life -= dt;
            p.position += p.velocity * dt;
            p.velocity += glm::vec3(0.0f, 1.0f, 0.0f) * dt * 2.0f;
        }
        else {
            respawnParticle(p, emitterPos);
        }
    }
}

void ParticleSystem::Render(const glm::mat4& projection, const glm::mat4& view) {
    if (!m_active) return;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_shader.use();
    m_shader.setMat4("projection", projection);
    m_shader.setMat4("view", view);
    glBindVertexArray(m_VAO);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    for (const auto& p : m_particles) {
        if (p.life > 0.0f) {

            glm::mat4 model = glm::translate(glm::mat4(1.0f), p.position);
            model = glm::scale(model, glm::vec3(p.scale));
            m_shader.setMat4("model", model);
            m_shader.setFloat("alpha", p.life);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
    }
    glBindVertexArray(0);
    glDisable(GL_BLEND);
}

void ParticleSystem::respawnParticle(Particle& particle, const glm::vec3& emitterPos) {
    static std::default_random_engine rng{ std::random_device{}() };
    std::uniform_real_distribution<float> dist(-0.3f, 0.3f);
    particle.position = emitterPos + glm::vec3(dist(rng), 0.0f, dist(rng));
    particle.velocity = glm::vec3(dist(rng), 2.0f + dist(rng), dist(rng));
    particle.life = 1.0f + dist(rng) * 0.5f;
    particle.scale = 2.0f + dist(rng) * 1.0f;
}

unsigned int ParticleSystem::loadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 4);
    if (data) {
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    return textureID;
}
