#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "RollerCoaster.h"
#include "Model.h"
#include "Light.h"

class Cart {
public:
	Cart(RollerCoaster* rollercoaster, float speed);

	void Update(float deltaTime);
	void Render(const glm::mat4& projection, const glm::mat4& view, std::vector<PointLight> lights, glm::vec3 cameraPos);

private:
	RollerCoaster* m_rollerCoaster;
	Shader m_shader;
	Model m_model;

	float m_speed;
	float m_t;
	int m_currentCurveIndex;

	glm::vec3 m_position;
	glm::vec3 m_direction;

	unsigned int VAO, VBO, EBO;
	unsigned int m_indexCount;

	void updatePositionAndDirection();
	void InitializeBuffers();
};

