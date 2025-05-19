#pragma once

#include "Scenery.h"

class Ship : public Scenery{
public:
    Ship(const glm::vec3& position, float scale = 1.0f)
        : Scenery(".\\models\\scenery\\ship-medium.fbx", position, scale) {
    }

};
