#pragma once

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
class Sphere {
public:
	Sphere(const glm::vec3& position, float scale, const glm::vec3& color);

	void Render(const glm::mat4& projection, const glm::mat4& view);
	void RenderRaw();

	const glm::vec3& GetPickingColor() const { return m_pickingColor; }

	glm::vec3 getPosition() { return m_position; }
	float getScale() { return m_scale;  }
	unsigned int getVAO() { return m_VAO; }
	unsigned int getIndexCount() { return m_indexCount;  }

private:
	Shader m_shader;

	glm::vec3 m_position;
	float m_scale;
	glm::vec3 m_color;

	unsigned int m_VAO = 0;
	unsigned int m_VBO = 0;
	unsigned int m_EBO = 0;
	unsigned int m_indexCount = 0;


	//ColorPicking
	glm::vec3 m_pickingColor = glm::vec3(1.0f, 0.0f, 0.0f); //rood
};