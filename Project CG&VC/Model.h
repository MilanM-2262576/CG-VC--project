#pragma once

#include <glm/glm.hpp>

#include "Shader.h"

#include <string>
#include <cstring>

class Model {
public:
	Model(const std::string& path);
	void Draw(Shader& shader);
	unsigned int LoadTexture(const char* path);

private:
	std::string m_directory;

	unsigned int VAO, VBO, EBO;
	unsigned int indexCount;
	unsigned int textureID;

	unsigned int LoadTextureFromMemory(unsigned char* data, unsigned int size);

};

