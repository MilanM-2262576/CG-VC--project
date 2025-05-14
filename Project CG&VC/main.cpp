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

// Screen size
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

//Grass
unsigned int loadTexture(const char* path);


//Camera
Camera camera(glm::vec3(0.0f, 100.0f, 3.0f)); // Hoger, zodat je op het grasveld neerkijkt
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

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
		// Lange rechte start
		{
			{0.0f, 1.0f, 0.0f},   // Startpunt
			{10.0f, 1.0f, 0.0f},  // Rechte lijn
			{20.0f, 1.0f, 0.0f},  // Rechte lijn
			{30.0f, 1.0f, 0.0f}   // Rechte lijn
		},
		// Zachte klim
		{
			{30.0f, 1.0f, 0.0f},
			{35.0f, 3.0f, 2.0f},  // Begin klim
			{40.0f, 5.0f, 4.0f},  // Top van de klim
			{45.0f, 3.0f, 6.0f}   // Overgang naar vlak stuk
		},
		// Zachte daling
		{
			{45.0f, 3.0f, 6.0f},
			{50.0f, 2.0f, 8.0f},  // Halverwege daling
			{55.0f, 1.5f, 10.0f}, // Bijna bodem
			{60.0f, 1.0f, 12.0f}  // Bodem
		},
		// Bocht naar rechts
		{
			{60.0f, 1.0f, 12.0f},
			{65.0f, 1.0f, 16.0f},  // Begin bocht
			{70.0f, 1.0f, 20.0f},  // Midden bocht
			{75.0f, 1.0f, 24.0f}   // Einde bocht
		},
		// Kleine klim en daling
		{
			{75.0f, 1.0f, 24.0f},
			{80.0f, 2.0f, 26.0f},  // Kleine klim
			{85.0f, 2.0f, 28.0f},  // Vlak stuk
			{90.0f, 1.0f, 30.0f}   // Kleine daling
		},
		// Bocht naar links
		{
			{90.0f, 1.0f, 30.0f},
			{95.0f, 1.0f, 34.0f},  // Begin bocht
			{100.0f, 1.0f, 38.0f}, // Midden bocht
			{105.0f, 1.0f, 42.0f}  // Einde bocht
		},
		// Zachte klim
		{
			{105.0f, 1.0f, 42.0f},
			{110.0f, 3.0f, 44.0f}, // Begin klim
			{115.0f, 5.0f, 46.0f}, // Top van de klim
			{120.0f, 3.0f, 48.0f}  // Overgang naar daling
		},
		// Zachte daling
		{
			{120.0f, 3.0f, 48.0f},
			{125.0f, 2.0f, 50.0f}, // Halverwege daling
			{130.0f, 1.5f, 52.0f}, // Bijna bodem
			{135.0f, 1.0f, 54.0f}  // Bodem
		},
		// Eindbocht
		{
			{135.0f, 1.0f, 54.0f},
			{140.0f, 1.0f, 58.0f},  // Begin bocht
			{145.0f, 1.0f, 62.0f},  // Midden bocht
			{150.0f, 1.0f, 66.0f}   // Einde bocht
		},
		// Extra segmenten om de achtbaan te verlengen
		// Rechte lijn
		{
			{150.0f, 1.0f, 66.0f},
			{160.0f, 1.0f, 70.0f},
			{170.0f, 1.0f, 74.0f},
			{180.0f, 1.0f, 78.0f}
		},
		// Zachte klim
		{
			{180.0f, 1.0f, 78.0f},
			{185.0f, 3.0f, 80.0f},
			{190.0f, 5.0f, 82.0f},
			{195.0f, 3.0f, 84.0f}
		},
		// Zachte daling
		{
			{195.0f, 3.0f, 84.0f},
			{200.0f, 2.0f, 86.0f},
			{205.0f, 1.5f, 88.0f},
			{210.0f, 1.0f, 90.0f}
		},
		// Bocht naar rechts
		{
			{210.0f, 1.0f, 90.0f},
			{215.0f, 1.0f, 94.0f},
			{220.0f, 1.0f, 98.0f},
			{225.0f, 1.0f, 102.0f}
		},
		// Eindbocht
		{
			{225.0f, 1.0f, 102.0f},
			{230.0f, 1.0f, 106.0f},
			{235.0f, 1.0f, 110.0f},
			{240.0f, 1.0f, 114.0f}
		}
	};

	RollerCoaster rollerCoaster(bezierSegments, 0.5f, 16); // Create a rollercoaster

	Cart cart(&rollerCoaster, 0.5f); // Create a cart

	// Create Heightmap
	Heightmap heightmap(".\\heightmap.png", ".\\sand.jpg", 64.0f / 256.0f, 16.0f);

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

		// Render the heightmap
		glm::mat4 heightmapModel = glm::mat4(1.0f);
		heightmap.Render(projection, view, heightmapModel);

		// Render the rollercoaster
		rollerCoaster.Render(projection, view);

		// Render the cart
		cart.Update(deltaTime);
		cart.Render(projection, view);


		glfwPollEvents();
		glfwSwapBuffers(window);
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