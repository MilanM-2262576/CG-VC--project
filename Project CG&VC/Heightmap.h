#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Shader.h"
#include "Light.h"
#include "stb_image.h"

#include <iostream>
#include <vector>
#include <string>

class Heightmap {
public:
	Heightmap(const std::string& heightmapPath, const std::string& texturePath, float yScale, float yShift);
	~Heightmap();

	void Render(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model);

private:
	void LoadHeightmap(const std::string& heightmapPath, float yScale, float yShift);
	void GenerateBuffers();
	unsigned int LoadTexture(const std::string& path);
	glm::vec3 computeNormal(int x, int z, int width, int height, 
							const std::vector<unsigned char >& heightData, float yScale, float yShift);

	unsigned int VAO, VBO, EBO;
	unsigned int sandTextureID, grassTextureID, rockTextureID, snowTextureID;
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	unsigned int numStrips, numVertsPerStrip;
};