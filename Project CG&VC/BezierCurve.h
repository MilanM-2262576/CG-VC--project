#pragma once

#include <glm/glm.hpp>
#include <vector>

/*
* This class represents the Bezier curve 
* containts:
*		- vector of points = the control points of the bezier curve
*/
class BezierCurve {
public:
	BezierCurve(const std::vector<glm::vec3>& controlPoints);

	void ForwardDifference(int steps);
	glm::vec3 GetPoint(float t) const;
	std::vector<glm::vec3> GeneratePoints(int resolution) const;

	void AddPoint(glm::vec3 point) { m_curvePoints.push_back(point); }
	void ClearPoints() { m_curvePoints.clear(); }

private:
	std::vector<glm::vec3> m_controlPoints;
	std::vector<glm::vec3> m_curvePoints;

};

