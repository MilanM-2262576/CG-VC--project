#include "ChromaKey.h"
#include "stb_image.h"


ChromaKey::ChromaKey(unsigned int width, unsigned int height, const char* overlayPath)
    : m_shader(".\\ChromaKey.vert", ".\\ChromaKey.frag"), m_width(width), m_height(height)
{
    
    stbi_set_flip_vertically_on_load(true);

    int tw, th, tc;
    unsigned char* data = stbi_load(overlayPath, &tw, &th, &tc, 4);
    if (!data) {
        std::cerr << "Failed to load overlay image: " << overlayPath << "\n" << stbi_failure_reason() << std::endl;
    }
    else {
        glGenTextures(1, &m_overlayTexture);
        glBindTexture(GL_TEXTURE_2D, m_overlayTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }

    InitQuad();
}


void ChromaKey::InitQuad() {
    // Stel breedte en hoogte van de overlay in pixels in
    const float overlayWidth = 400.0f;
    const float overlayHeight = 200.0f;

    // Zet om naar NDC (x: -1 tot 1, y: -1 tot 1)
    float left = -1.0f;
    float right = -1.0f + 2.0f * (overlayWidth / 1920);
    float top = 1.0f;
    float bottom = 1.0f - 2.0f * (overlayHeight / 1080);

    float quadVertices[] = {
        // positions      // texCoords
        left,    top,     0.0f, 1.0f,
        left,    bottom,  0.0f, 0.0f,
        right,   bottom,  1.0f, 0.0f,

        left,    top,     0.0f, 1.0f,
        right,   bottom,  1.0f, 0.0f,
        right,   top,     1.0f, 1.0f
    };
    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &m_quadVBO);
    glBindVertexArray(m_quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);
}

void ChromaKey::Render(GLuint sceneTexture) {
    m_shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sceneTexture);
    m_shader.setInt("sceneTexture", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_overlayTexture);
    m_shader.setInt("overlayTexture", 1);

    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
