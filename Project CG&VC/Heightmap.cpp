#include "Heightmap.h"

Heightmap::Heightmap(const std::string& heightmapPath, const std::string& texturePath, float yScale, float yShift) {
    LoadHeightmap(heightmapPath, yScale, yShift);
    GenerateBuffers();

    // Derive texture paths from texturePath base
    std::string sandPath = texturePath + "/sand_cartoon.jpg";
    std::string grassPath = texturePath + "/grass_cartoon.jpg";
    std::string rockPath = texturePath + "/rock_cartoon.jpg";
    std::string snowPath = texturePath + "/snow_cartoon.jpg";

    sandTextureID = Utilities::loadTexture(sandPath);
    grassTextureID = Utilities::loadTexture(grassPath);
    rockTextureID = Utilities::loadTexture(rockPath);
    snowTextureID = Utilities::loadTexture(snowPath);
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

    std::vector<unsigned char> heightData(width * height);
    for (int i = 0; i < width * height; ++i) {
        heightData[i] = data[i * nChannels];
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
            float tilingFactor = 60.0f; // Used to increase amount of textures uses
            vertices.push_back((float)j / (width - 1) * tilingFactor);        // u
            vertices.push_back((float)i / (height - 1) * tilingFactor);       // v

            // Normal
            glm::vec3 normal = computeNormal(j, i, width, height, heightData, yScale, yShift);
            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);
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

glm::vec3 Heightmap::computeNormal(int x, int z, int width, int height, const std::vector<unsigned char >& heightData, float yScale, float yShift) {
    auto getHeight = [&](int i, int j) -> float {
        i = glm::clamp(i, 0, width - 1);
        j = glm::clamp(j, 0, height - 1);
        return static_cast<float>(heightData[j * width + i]) * yScale - yShift;
        };

    float hl = getHeight(x - 1, z);
    float hr = getHeight(x + 1, z);
    float hd = getHeight(x, z - 1);
    float hu = getHeight(x, z + 1);

    glm::vec3 normal = glm::normalize(glm::vec3(hl - hr, 2.0f, hd - hu));
    return normal;
}

void Heightmap::GenerateBuffers() {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void Heightmap::Render(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model) {
    Shader heightmapShader(".\\heightmapShader.vert", ".\\heightmapShader.frag");
    heightmapShader.use();

    heightmapShader.setInt("sandTexture", 0);
    heightmapShader.setInt("grassTexture", 1);
    heightmapShader.setInt("rockTexture", 2);
    heightmapShader.setInt("snowTexture", 3);

    heightmapShader.setMat4("projection", projection);
    heightmapShader.setMat4("view", view);
    heightmapShader.setMat4("model", model);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sandTextureID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, grassTextureID);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, rockTextureID);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, snowTextureID);

    glBindVertexArray(VAO);
    for (unsigned int strip = 0; strip < numStrips; ++strip) {
        glDrawElements(GL_TRIANGLE_STRIP, numVertsPerStrip, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * numVertsPerStrip * strip));
    }
}


float Heightmap::GetHeightAt(float x, float z) const {
    int width = static_cast<int>(sqrt(vertices.size() / 8)); 
    int height = width;
    float fx = x + width / 2.0f;
    float fz = z + height / 2.0f;
    int ix = static_cast<int>(fx);
    int iz = static_cast<int>(fz);
    if (ix < 0 || ix >= width || iz < 0 || iz >= height) return 0.0f;
    size_t idx = (iz * width + ix) * 8 + 1; 
    return vertices[idx];
}
