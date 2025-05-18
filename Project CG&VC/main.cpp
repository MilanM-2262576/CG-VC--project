#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"

#include <iostream>

#include "Camera.h"
#include "Shader.h"
#include "Heightmap.h"
#include "Water.h"
#include "BezierCurve.h"
#include "Rollercoaster.h"
#include "Cart.h"

#include "Light.h"
#include "Utilities.h"
#include "SkyBox.h"

// Screen size
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

//Camera
Camera camera(glm::vec3(0.0f, 100.0f, 3.0f)); // Pretty high for now so you can see everything better
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// lighting
std::vector<glm::vec3> lightPos = {
	{ 20.0f, 75.0f, 0.0f },
	{ 110.0f, 40.0f, 110.0f },
	{ 110.0f, 35.0f, -110.0f },
	{ -110.0f, 40.0f, -110.0f },
	{ 75.0f, 45.0f, 20.0f },
	{ -10.0f, 45.0f, -90.0f },
};

std::vector<glm::vec3> lightColor = {
	{ 1.0f, 0.5f, 0.0f },	// Orange
	{ 0.0f, 1.0f, 1.0f },	// Cyan
	{ 1.0f, 0.0f, 1.0f },	// Magenta
	{ 0.2f, 1.0f, 0.2f },	// Bright green
	{ 0.2f, 0.4f, 1.0f },	// Blue
	{ 1.0f, 1.0f, 0.3f },	// Yellow
};

// Floats deciding how quickly the light fades over a distance
float constant = 1.0f;
float linear = 0.005f;
float quadratic = 0.00015f;

//FBO variables
unsigned int fbo = 0;
unsigned int fboTexture = 0;
unsigned int fboRBO = 0;

//functions
void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
GLFWwindow* InitializeGLFW();

int main() {
	//Initialize GLFW window
	GLFWwindow* window = InitializeGLFW();
	if (!window) { return -1; }

	SetupFBO();

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

	// Create the rollercoaster
	RollerCoaster rollerCoaster(bezierSegments, 0.5f, 16); // Create a rollercoaster

	// Create a cart
	Cart cart(&rollerCoaster, 0.2f); // Create a cart

	// Create Heightmap
	Heightmap heightmap(".\\heightmap.jpeg", ".\\textures", 64.0f / 256.0f, 16.0f);

	// Create water plane
	Water water(0.0f, ".\\heightmap.jpeg");

	// Create lights
	std::vector<PointLight> pointLights;
	std::vector<Light> lights;
	for (size_t i = 0; i < lightPos.size() && lightColor.size(); ++i) {
		pointLights.push_back({ lightPos[i], lightColor[i], constant, linear, quadratic});
		lights.emplace_back(lightPos[i], ".\\models\\lamp\\JapaneseLamp.obj", lightColor[i]);
	}

	SkyBox skybox(".\\SkyBoxShader.vert", ".\\SkyBoxShader.frag");

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
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Projection en view matrices
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		glm::mat4 view = camera.GetViewMatrix();

		// Update and render the lightsources
		for (size_t i = 0; i < lights.size(); ++i) {
			lights[i].Update(currentFrame);
		}

		for (size_t i = 0; i < pointLights.size() && i < lights.size(); ++i) {
			pointLights[i].position = lights[i].position;
			pointLights[i].color = lights[i].color;
			lights[i].Render(projection, view);
		}

		// Render the rollercoaster
		rollerCoaster.Render(projection, view);

		// Render the cart
		cart.Update(deltaTime);
		cart.Render(projection, view, pointLights, camera.Position);

		if (camera.cameraOption == 1)
			camera.UpdateCartCamera(cart.GetPosition(), cart.GetDirection());

		// Render the heightmap
		glm::mat4 heightmapModel = glm::mat4(1.0f);
		heightmap.Render(projection, view, heightmapModel);
		
		// Render the water (possible to expand with more details etc.)
		water.Render(projection, view);

		// Render the SkyBox
		skybox.Render(projection, view);

		//Poll for events
		glfwPollEvents();
		glfwSwapBuffers(window);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	// Clean up
	rollerCoaster.CleanUp();

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
	glfwSetKeyCallback(window, key_callback);
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
	if (camera.cameraOption == 0) {
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.ProcessKeyboard(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.ProcessKeyboard(BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.ProcessKeyboard(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.ProcessKeyboard(RIGHT, deltaTime);
	}
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

	if (camera.cameraOption != 1)
		camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	std::cout << key << "pressed" << std::endl;
	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
		camera.ChangeOption();
};

// glfw: whenever the window size changed, this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void SetupFBO() {
	// Create framebuffer
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Create texture to hold color buffer
	glGenTextures(1, &fboTexture);
	glBindTexture(GL_TEXTURE_2D, fboTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0);

	// Create renderbuffer for depth and stencil
	glGenRenderbuffers(1, &fboRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, fboRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fboRBO);

	// Check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}