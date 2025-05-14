#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"

#include <iostream>

#include "Camera.h"
#include "Shader.h"
#include "Heightmap.h"
#include "BezierCurve.h"
#include "Rollercoaster.h"
#include "Cart.h"
#include "Light.h"

// Screen size
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

//Grass
unsigned int loadTexture(const char* path);


//Camera
Camera camera(glm::vec3(0.0f, 100.0f, 3.0f)); // Pretty high for now so you can see the heightmap better
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 101.0f, 2.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

// Floats deciding how quickly the light fades over a distance
float constant = 1.0f;
float linear = 0.09f;
float quadratic = 0.032f;

//functions
void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
GLFWwindow* InitializeGLFW();


int main() {
	//Initialize GLFW window
	GLFWwindow* window = InitializeGLFW();
	if (!window) { return -1; }


	std::vector<std::vector<glm::vec3>> bezierSegments = {
		// Start links-voor
		{
			{-120.0f, 30.0f, -120.0f},
			{-80.0f, 35.0f, -80.0f},
			{-40.0f, 50.0f, -40.0f},   // Begin klim
			{0.0f, 70.0f, 0.0f}        // Top van de berg
		},
		// Midden naar rechts-achter (afdaling)
		{
			{0.0f, 70.0f, 0.0f},       // Top van de berg
			{40.0f, 50.0f, 40.0f},     // Daling
			{80.0f, 35.0f, 80.0f},
			{120.0f, 30.0f, 120.0f}
		},
		// Bocht naar links-achter
		{
			{120.0f, 30.0f, 120.0f},
			{100.0f, 40.0f, 60.0f},
			{60.0f, 35.0f, 100.0f},
			{120.0f, 30.0f, -120.0f}
		},
		// Bocht naar rechts-voor en terug naar start
		{
			{120.0f, 30.0f, -120.0f},
			{60.0f, 40.0f, -100.0f},
			{-60.0f, 35.0f, -100.0f},
			{-120.0f, 30.0f, -120.0f}
		}
	};


	RollerCoaster rollerCoaster(bezierSegments, 0.5f, 16); // Create a rollercoaster

	Cart cart(&rollerCoaster, 0.5f); // Create a cart

	// Create Heightmap
	Heightmap heightmap(".\\heightmap.jpeg", ".\\textures", 64.0f / 256.0f, 16.0f);

	std::vector<PointLight> lights = {
		{ lightPos, lightColor, constant, linear, quadratic }
	};

	Light light(lightPos, lightColor);
	light.Initialize();

	Shader lightingShader(".\\LightingShader.vert", ".\\LightingShader.frag");

	float cubeVertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	unsigned int lightingVAO, lightingVBO;
	glGenBuffers(1, &lightingVBO);
	glGenVertexArrays(1, &lightingVAO);
	glBindVertexArray(lightingVAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, lightingVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	while (!glfwWindowShouldClose(window)) {
		// Time 
		// -------------------------
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Process inputs
		// -------------------------
		processInput(window);

		// Render
		// --------------------------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Projection en view matrices
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		glm::mat4 view = camera.GetViewMatrix();

		// Render the rollercoaster
		rollerCoaster.Render(projection, view);

		// Render the cart
		cart.Update(deltaTime);
		cart.Render(projection, view);


		light.Update(currentFrame);

		// Should be changed when placing multiple lights/lamps (right now only one cube as lightsource)
		lights[0].position = light.position;
		lights[0].color = light.color;

		light.Render(projection, view);

		lightingShader.use();
		lightingShader.setInt("numLights", lights.size());
		for (size_t i = 0; i < lights.size(); ++i) {
			std::string idx = std::to_string(i);
			lightingShader.setVec3("lights[" + idx + "].position", lights[i].position);
			lightingShader.setVec3("lights[" + idx + "].color", lights[i].color);
			lightingShader.setFloat("lights[" + idx + "].constant", lights[i].constant);
			lightingShader.setFloat("lights[" + idx + "].linear", lights[i].linear);
			lightingShader.setFloat("lights[" + idx + "].quadratic", lights[i].quadratic);
		}

		lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		lightingShader.setVec3("viewPos", camera.Position);

		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);

		glm::mat4 lightingModel = glm::mat4(1.0f);
		lightingModel = glm::translate(lightingModel, glm::vec3(0.0f, 100.0f, 0.0f));
		lightingShader.setMat4("model", lightingModel);

		glBindVertexArray(lightingVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Render the heightmap
		glm::mat4 heightmapModel = glm::mat4(1.0f);
		heightmap.Render(projection, view, heightmapModel);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	// Clean up
	rollerCoaster.CleanUp();

	glDeleteBuffers(1, &lightingVBO);
	glDeleteVertexArrays(1, &lightingVAO);

	glfwTerminate();
	return 0;
}

//Initialization of GLFW
GLFWwindow* InitializeGLFW() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CG&VC project", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return nullptr;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return nullptr;
	}

	glEnable(GL_DEPTH_TEST);
	return window;
}

// this functions processes each input
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// camera controls
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// glfw: whenever the window size changed, this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}


// This method loads the texture from the given path
unsigned int loadTexture(const char* path) {
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