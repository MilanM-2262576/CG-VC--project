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
    
    glGenTextures(1, &m_overlayTexture);
    glBindTexture(GL_TEXTURE_2D, m_overlayTexture);
    GLenum format = (tc == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);

    InitQuad();
}

void ChromaKey::InitQuad() {
    // Overlay size in pixels (dog image size)
    const float overlayWidth = 400.0f;
    const float overlayHeight = 200.0f;

    // Convert to NDC
    float left = -1.0f;
    float right = left + 2.0f * (overlayWidth / m_width);
    float bottom = -1.0f;
    float top = bottom + 2.0f * (overlayHeight / m_height);

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

void ChromaKey::Render() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_overlayTexture);
    m_shader.setInt("overlayTexture", 0);

    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glDisable(GL_BLEND);
}
