#pragma once

#include <string>
#include "Shader.h"
#include <glm/glm.hpp>

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

};

