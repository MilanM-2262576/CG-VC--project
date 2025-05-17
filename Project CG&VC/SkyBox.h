#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "Shader.h"

class SkyBox {
public:
    SkyBox(const char* vertPath, const char* fragPath);
    ~SkyBox();

    void Render(const glm::mat4& projection, const glm::mat4& view);

private:
    unsigned int VAO, VBO;
    unsigned int cubemapTexture;
    Shader shader;

    unsigned int loadCubemap(const std::vector<std::string>& faces);
};