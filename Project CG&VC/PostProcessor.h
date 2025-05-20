#pragma once

#include "Shader.h"
#include "PostProcessKernel.h"

class PostProcessor {
public:
    PostProcessor(unsigned int width, unsigned int height, const char* vertPath, const char* fragPath);
    ~PostProcessor();

    void StartRender() const;
    void EndRender(const PostProcessKernel& kernel, float offset = 1.0f / 300.0f);

    unsigned int GetTexture() const;
    unsigned int GetColorTexture() const;

private:
    void InitFBO();
    void InitScreenQuad();

    unsigned int m_width, m_height;
    unsigned int m_fbo, m_fboTexture, m_fboRBO;
    unsigned int m_quadVAO, m_quadVBO;
    Shader m_shader;
};