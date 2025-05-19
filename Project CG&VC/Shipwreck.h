#pragma once

#include "Scenery.h"

class Shipwreck : public Scenery {
public:
    Shipwreck(const glm::vec3& position, float scale = 1.0f)
        : Scenery(".\\models\\scenery\\ship-wreck.fbx", position, scale) {
    }

};
