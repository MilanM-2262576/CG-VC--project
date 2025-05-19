#pragma once
#include "Scenery.h"

class Boat : public Scenery {
public:
    Boat(const glm::vec3& position, float scale = 1.0f)
        : Scenery(".\\models\\scenery\\boat-row-small.fbx", position, scale) {
    }

};
