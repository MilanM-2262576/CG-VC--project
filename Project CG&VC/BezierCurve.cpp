#include "BezierCurve.h"

#include <stdexcept>

/* 
* Constructor for the BezierCurve class
* initializes the controlPoints
*/
BezierCurve::BezierCurve(const std::vector<glm::vec3>& controlPoints) {
    if (controlPoints.size() != 4)
        throw std::runtime_error("BezierCurve requires exactly 4 control points.");

    m_controlPoints = controlPoints;
}

/*
* ForwardDifference calculates the points of the Bézier curve using the forward difference method
*/
void BezierCurve::ForwardDifference(int steps) {
    if (m_controlPoints.size() != 4) {
        throw std::runtime_error("ForwardDifference only supports cubic Bézier curves (==4 control points).");
    }
    if (steps <= 0) {
        throw std::runtime_error("Steps must be greater than 0.");
    }

    // calculate the Forward Difference-coëfficiënts
    glm::vec3 p0 = m_controlPoints[0];
    glm::vec3 p1 = m_controlPoints[1];
    glm::vec3 p2 = m_controlPoints[2];
    glm::vec3 p3 = m_controlPoints[3];
     
	// calculate coefficients of bezier curve  -- B(t) = at^3 + bt^2 + ct + d
    glm::vec3 a = -p0 + 3.0f * (p1 - p2) + p3;          // -p0 + 3*(p1 - p2) + p3
    glm::vec3 b = 3.0f * (p0 - 2.0f * p1 + p2);         // 3*(p0 - 2*p1 + p2)
	glm::vec3 c = 3.0f * (p1 - p0);                     // 3*(p1 - p0)
	glm::vec3 d = p0;                                   // startpoint t0

    float h = 1.0f / steps; //h
    float h2 = h * h;       // h^2
    float h3 = h * h * h;      // h^3

    // Forward Difference-coëfficiënten
    // B(t) = B(t0) + delta1 * h + delta2 * h^2 + delta3 * h^3
	glm::vec3 delta1 = a * h3 + b * h2 + c * h;         // delta1 = B'(t0) * h
	glm::vec3 delta2 = 6.0f * a * h3 + 2.0f * b * h2;   // delta2 = B''(t0) * h^2
	glm::vec3 delta3 = 6.0f * a * h3;                    // delta3 = B'''(t0) * h^3      

    // start with startpoint t0
    glm::vec3 point = d; 
    AddPoint(point);

    // calculate other points with forward differencing 
    for (int i = 1; i <= steps; ++i) {
        point += delta1;
        delta1 += delta2;
        delta2 += delta3;
        AddPoint(point);
    }
}

//GeneratePoints calculates the points of the Bézier curve using the forward difference method
std::vector<glm::vec3> BezierCurve::GeneratePoints(int resolution) {
    ClearPoints();
    ForwardDifference(resolution);
    return m_curvePoints;
}


glm::vec3 BezierCurve::GetPoint(float t) const {
    if (m_controlPoints.size() != 4) {
        throw std::runtime_error("GetPoint only supports cubic Bézier curves (4 control points).");
    }

    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    glm::vec3 point = uuu * m_controlPoints[0]; // (1-t)^3 * P0
    point += 3 * uu * t * m_controlPoints[1];  // 3 * (1-t)^2 * t * P1
    point += 3 * u * tt * m_controlPoints[2];  // 3 * (1-t) * t^2 * P2
    point += ttt * m_controlPoints[3];         // t^3 * P3

    return point;
}

