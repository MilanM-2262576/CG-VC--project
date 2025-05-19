#pragma once

#include <vector>
#include <glm/glm.hpp>

class BezierTrack {
public:
    BezierTrack(const std::vector<std::vector<glm::vec3>>& segments) : m_segments(segments) {
		MakeSegmentsSmooth();
    }

	void MakeSegmentsSmooth() {
		for (size_t i = 1; i < m_segments.size(); ++i) {
			// Vorig segment
			auto& prev = m_segments[i - 1];
			// Huidig segment
			auto& curr = m_segments[i];
			// Zorg dat begin/eindpunten gelijk zijn
			curr[0] = prev[3];
			// Pas de eerste control point van het huidige segment aan voor C1-continuïteit
			curr[1] = curr[0] + (prev[3] - prev[2]);
		}

		auto& last = m_segments.back();
		auto& first = m_segments.front();
		// Eindpunt laatste segment = beginpunt eerste segment (is al zo)
		first[0] = last[3];
		// Pas de eerste control point van het eerste segment aan
		first[1] = first[0] + (last[3] - last[2]);
	}

    std::vector<std::vector<glm::vec3>>& GetSegments() { return m_segments; }
    const std::vector<std::vector<glm::vec3>>& GetSegments() const { return m_segments; }

private:
    std::vector<std::vector<glm::vec3>> m_segments;
};
