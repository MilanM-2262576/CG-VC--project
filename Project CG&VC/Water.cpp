#include "Water.h"

Water::Water(float seaLevel, const std::string heightmapPath) : seaLevel(seaLevel) {
	int width, height, nChannels;
	unsigned char* data = stbi_load(heightmapPath.c_str(), &width, &height, &nChannels, 0);
	if (!data) {
		std::cerr << "Failed to load heightmap: " << heightmapPath << std::endl;
		return;
	}

	float vertices[] = {
		// positions              // texcoords
		-width / 2, seaLevel, -height / 2, 0.0f, 0.0f, // triangle 1
		 width / 2, seaLevel, -height / 2, 1.0f, 0.0f,
		 width / 2, seaLevel,  height / 2, 1.0f, 1.0f,

		-width / 2, seaLevel, -height / 2, 0.0f, 0.0f, // triangle 2
		 width / 2, seaLevel,  height / 2, 1.0f, 1.0f,
		-width / 2, seaLevel,  height / 2, 0.0f, 1.0f
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); // Position
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); // Texcoords
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	waterTextureID = loadTexture(".\\textures\\water.jpg");
}

void Water::Render(const glm::mat4& projection, const glm::mat4& view) const {
	Shader waterShader(".\\waterShader.vert", ".\\waterShader.frag");
	waterShader.use();

	waterShader.setMat4("projection", projection);
	waterShader.setMat4("view", view);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, seaLevel + 0.01f, 0.0f));
	waterShader.setMat4("model", model);

	waterShader.setVec3("waterColor", glm::vec3(0.0f, 0.3f, 0.8f));
	waterShader.setFloat("transparency", 0.5f);

	waterShader.setInt("waterTexture", 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterTextureID);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(-1.0f, -1.0f);	// Negative values push the water closer to the camera

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisable(GL_POLYGON_OFFSET_FILL);
}

// This method loads the texture from the given path
unsigned int Water::loadTexture(const char* path) {
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		std::cout << "Failed to load texture: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}