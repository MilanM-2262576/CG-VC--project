#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "Shader.h"
#include "BezierCurve.h"

// This class represents the rollercoaster which consists of multiple Bezier curves.
// Each Bezier curve is represented by a series of control points.
class RollerCoaster {
public:
	RollerCoaster(std::vector<std::vector<glm::vec3>> bezierSegments, float cylinderRadius, int cylinderSegments);
	~RollerCoaster();

	void Render(const glm::mat4& projection, const glm::mat4& view);
	void CleanUp();

	std::vector<BezierCurve>& getCurves() { return m_curves; }


private:
	std::vector<BezierCurve> m_curves;
	std::vector<unsigned int> m_cylinderVAOs;
	float m_cylinderRadius;
	int m_cylinderSegments;

	Shader m_shader;

	void GenerateCylinders();
	unsigned int createCylinderVAO(const glm::vec3& start, const glm::vec3& end, float radius, int segments);


};

