#pragma once
#include <glm/glm.hpp>
#include "Shader.h"
#include "Sphere.h"

class ColorPicker {
public:
    ColorPicker(unsigned int width, unsigned int height);

    // Render het object voor picking
    void Render(const glm::mat4& projection, const glm::mat4& view, Sphere& sphere);

    // Voer picking uit op schermcoördinaat (x, y)
    bool Pick(int x, int y, const glm::mat4& projection, const glm::mat4& view, Sphere& sphere);

private:
    unsigned int m_fbo = 0;
    unsigned int m_texture = 0;
    unsigned int m_width, m_height;
    Shader m_shader;
};
