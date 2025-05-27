#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "stb_image.h"

#include "Shader.h"
#include "Utilities.h"

class Water {
public:
	Water(float seaLevel, const std::string heightmapPath);
	void Render(const glm::mat4& projection, const glm::mat4& view);

	void SetTime(float time);

private:
	Shader m_shader;

	unsigned int VAO, VBO;
	unsigned int waterTextureID;
	float seaLevel;
};