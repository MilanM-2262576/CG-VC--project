#include "BezierCurve.h"

#include <stdexcept>

/* 
* Constructor for the BezierCurve class
* initializes the controlPoints
*/
BezierCurve::BezierCurve(const std::vector<glm::vec3>& controlPoints) : m_controlPoints(controlPoints) {
    if (controlPoints.size() != 4)
        throw std::runtime_error("BezierCurve requires exactly 4 control points.");
}

void BezierCurve::ForwardDifference(int steps) {
    if (m_controlPoints.size() < 2) { return;  } //minimum of 2 points is needed
    if (steps <= 0) { return;  }                 //steps > 0 is needed

    m_curvePoints.clear();

    // calculate step size
    float stepSize = 1.0f / steps;

    // calculate the Forward Difference-coëfficiënts
    glm::vec3 p0 = m_controlPoints[0];
    glm::vec3 p1 = m_controlPoints[1];
    glm::vec3 p2 = m_controlPoints[2];
    glm::vec3 p3 = m_controlPoints[3];

    glm::vec3 f0 = p0;
    glm::vec3 f1 = 3.0f * (p1 - p0) * stepSize;
    glm::vec3 f2 = 3.0f * (p0 - 2.0f * p1 + p2) * stepSize * stepSize;
    glm::vec3 f3 = (p3 - 3.0f * p2 + 3.0f * p1 - p0) * stepSize * stepSize * stepSize;

    // Iteratief punten genereren
    glm::vec3 point = f0;
    glm::vec3 delta1 = f1;
    glm::vec3 delta2 = f2;
    glm::vec3 delta3 = f3;

    for (int i = 0; i <= steps; ++i) {
        AddPoint(point); // Voeg het huidige punt toe aan de curve
        point += delta1;
        delta1 += delta2;
        delta2 += delta3;
    }
}

glm::vec3 BezierCurve::GetPoint(float t) const {
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

std::vector<glm::vec3> BezierCurve::GeneratePoints(int resolution) const {
    std::vector<glm::vec3> points;
    for (int i = 0; i <= resolution; ++i) {
        float t = static_cast<float>(i) / resolution;
        points.push_back(GetPoint(t));
    }
    return points;
}