#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "stb_image.h"

#include <iostream>

class Utilities {
public:
	static unsigned int loadTexture(const std::string path);
};