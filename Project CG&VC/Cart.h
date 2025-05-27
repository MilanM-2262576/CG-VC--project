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

	glm::vec3 GetPosition() const;
	glm::vec3 GetDirection() const;

	Shader getShader() { return m_shader; }


private:
	RollerCoaster* m_rollerCoaster;
	Shader m_shader;
	Model m_model;
	Model m_wagonModel;

	float m_velocity = 0.0f;
	float m_speed;
	float m_t;
	int m_currentCurveIndex;

	float m_mass = 100.0f;

	glm::vec3 m_position;
	glm::vec3 m_direction;

	unsigned int VAO, VBO, EBO;
	unsigned int m_indexCount;

	void updatePositionAndDirection();
	void InitializeBuffers();
};

