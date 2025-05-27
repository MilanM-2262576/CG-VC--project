#include "ColorPicker.h"
#include <glad/glad.h>

ColorPicker::ColorPicker(unsigned int width, unsigned int height)
    : m_width(width), m_height(height), m_shader(".\\PickingShader.vert", ".\\PickingShader.frag")
{
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void ColorPicker::Render(const glm::mat4& projection, const glm::mat4& view, Sphere& sphere) {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_shader.use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, sphere.getPosition());
    model = glm::scale(model, glm::vec3(sphere.getScale()));
    m_shader.setMat4("projection", projection);
    m_shader.setMat4("view", view);
    m_shader.setMat4("model", model);
    m_shader.setVec3("pickingColor", glm::vec3(1.0f, 0.0f, 0.0f)); // Rood

    glBindVertexArray(sphere.getVAO());
    glDrawElements(GL_TRIANGLES, sphere.getIndexCount(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool ColorPicker::Pick(int x, int y, const glm::mat4& projection, const glm::mat4& view, Sphere& sphere) {
    Render(projection, view, sphere);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    unsigned char data[3];
    glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, data);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Check op rood (255,0,0)
    return (data[0] == 255 && data[1] == 0 && data[2] == 0);
}
