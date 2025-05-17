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

	waterTextureID = Utilities::loadTexture(".\\textures\\water.jpg");
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