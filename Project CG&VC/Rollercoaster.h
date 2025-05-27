#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "Shader.h"
#include "BezierCurve.h"

// This class represents the rollercoaster which consists of multiple Bezier curves.
// Each Bezier curve is represented by a series of control points.
class RollerCoaster {
public:
	RollerCoaster(std::vector<std::vector<glm::vec3>> bezierSegments, int cylinderSegments);
	~RollerCoaster();

	void Render(const glm::mat4& projection, const glm::mat4& view);
	void CleanUp();

	std::vector<BezierCurve>& getCurves() { return m_curves; }

	Shader getShader() { return m_shader;  }


private:
	std::vector<BezierCurve> m_curves;
	std::vector<unsigned int> m_cylinderVAOs;
	float m_cylinderRadius = 0.25f;
	int m_cylinderSegments;
	int m_trackWidth = 2.0f;
	float m_crossbarThickness = 0.25f;

	Shader m_shader;

	void GenerateCylinders();
	void GeneratePillars(float interval);
	unsigned int createCylinderVAO(const glm::vec3& start, const glm::vec3& end, float radius, int segments);
	unsigned int createCrossbarVAO(const glm::vec3& left, const glm::vec3& right, float thickness);



};

