#pragma once

#include <glm/glm.hpp>

#include "Shader.h"

#include <string>
#include <cstring>
#include "Shader.h"

class Model {
public:
	Model(const std::string& path);
	void Draw(Shader& shader);
	unsigned int LoadTexture(const char* path);

private:
	std::string m_directory;

	bool m_useTexture = true;

	unsigned int VAO, VBO, EBO;
	unsigned int indexCount;
	unsigned int textureID;


};

