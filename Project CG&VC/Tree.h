#pragma once

#include "Scenery.h"

class Tree : public Scenery {
public:
    Tree(const glm::vec3& position, float scale = 1.0f)
        : Scenery(".\\models\\scenery\\tree.fbx", position, scale) {
    }

};
