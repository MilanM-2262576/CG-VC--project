#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Shader.h"

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    float life;    
    float scale;
};



class ParticleSystem {
public:
    ParticleSystem(unsigned int maxParticles, const char* texturePath);
    ~ParticleSystem();

    void Update(float dt, const glm::vec3& emitterPos);
    void Render(const glm::mat4& projection, const glm::mat4& view);

    void SetActive(bool active) { m_active = active; }
    bool IsActive() const { return m_active; }

private:
    std::vector<Particle> m_particles;

    unsigned int m_maxParticles;
    unsigned int m_VAO, m_VBO;
    unsigned int m_texture;

    Shader m_shader;
    bool m_active;

    void respawnParticle(Particle& particle, const glm::vec3& emitterPos);
    unsigned int loadTexture(const char* path);
};
