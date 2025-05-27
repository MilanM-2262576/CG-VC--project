#pragma once
#include "Scenery.h"

class Cannon : public Scenery {
public:
    Cannon(const glm::vec3& position, float scale = 1.0f)
        : Scenery(".\\models\\scenery\\cannon-mobile.fbx", position, scale) {
    }

};
