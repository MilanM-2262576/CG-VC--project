#pragma once

#include "Scenery.h"

class Tower : public Scenery {
public:
    Tower(const glm::vec3& position, float scale = 1.0f)
        : Scenery(".\\models\\scenery\\tower-complete-large.fbx", position, scale) {
    }

};
