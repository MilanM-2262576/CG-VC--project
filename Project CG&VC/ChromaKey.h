#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Shader.h"

class ChromaKey {
public:
    ChromaKey(unsigned int width, unsigned int height, const char* imagePath);


    void Render();


private:
    void InitQuad();
    GLuint m_overlayTexture;
    GLuint m_quadVAO, m_quadVBO;
    Shader m_shader;
    unsigned int m_width, m_height;


};

