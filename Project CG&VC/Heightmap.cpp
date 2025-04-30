#include "Heightmap.h"

Heightmap::Heightmap(const std::string& heightmapPath, const std::string& texturePath, float yScale, float yShift) {
    LoadHeightmap(heightmapPath, yScale, yShift);
    GenerateBuffers();
    textureID = LoadTexture(texturePath);
}

Heightmap::~Heightmap() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Heightmap::LoadHeightmap(const std::string& heightmapPath, float yScale, float yShift) {
    int width, height, nChannels;
    unsigned char* data = stbi_load(heightmapPath.c_str(), &width, &height, &nChannels, 0);
    if (!data) {
        std::cerr << "Failed to load heightmap: " << heightmapPath << std::endl;
        return;
    }

    for (unsigned int i = 0; i < height; ++i) {
        for (unsigned int j = 0; j < width; ++j) {
            unsigned char* texel = data + (j + width * i) * nChannels;
            unsigned char y = texel[0];

            // Vertex positions
            vertices.push_back(-width / 2.0f + j);             // v.x
            vertices.push_back((int)y * yScale - yShift);      // v.y
            vertices.push_back(-height / 2.0f + i);            // v.z

            // Texture coordinates
            float tilingFactor = 5.0f; // Used to increase amount of textures uses
            vertices.push_back((float)j / (width - 1) * tilingFactor);        // u
            vertices.push_back((float)i / (height - 1) * tilingFactor);       // v
        }
    }

    for (unsigned int i = 0; i < height - 1; ++i) {
        for (unsigned int j = 0; j < width; ++j) {
            for (unsigned int k = 0; k < 2; ++k) {
                indices.push_back(j + width * (i + k));
            }
        }
    }

    numStrips = height - 1;
    numVertsPerStrip = width * 2;

    stbi_image_free(data);
}

void Heightmap::GenerateBuffers() {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}

unsigned int Heightmap::LoadTexture(const std::string& path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format = (nrComponents == 1) ? GL_RED : (nrComponents == 3) ? GL_RGB : GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else {
        std::cerr << "Failed to load texture: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void Heightmap::Render(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model) {
    Shader heightmapShader(".\\heightmapShader.vert", ".\\heightmapShader.frag");
    heightmapShader.use();
    heightmapShader.setInt("heightmapTexture", 0);
    heightmapShader.setMat4("projection", projection);
    heightmapShader.setMat4("view", view);
    heightmapShader.setMat4("model", model);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glBindVertexArray(VAO);
    for (unsigned int strip = 0; strip < numStrips; ++strip) {
        glDrawElements(GL_TRIANGLE_STRIP, numVertsPerStrip, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * numVertsPerStrip * strip));
    }
}
