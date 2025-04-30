#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"

#include <iostream>

#include "Camera.h"
#include "Shader.h"
#include "Heightmap.h"
#include "BezierCurve.h"

// Screen size
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

//Grass
unsigned int loadTexture(const char* path);

//Bezier
unsigned int createLineVAO(const std::vector<glm::vec3>& points);

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
  
	//bezier controlpoints for the multiple Bezier segments
	std::vector<std::vector<glm::vec3>> bezierSegments = {
	{
		{-1.0, 1.0, -1.0},  
		{-0.5, 3.0, 0.0},   
		{0.5, 0.5, 0.0},    
		{1.0, 1.0, 0.5}     
	},
	{
		{1.0, 1.0, 0.5},   
		{1.5, 0.0, 1.0},    
		{2.0, 2.5, 1.5},   
		{2.5, 1.0, 2.0}   
	},
	{
		{2.5, 1.0, 2.0},   
		{3.0, 3.0, 2.5},   
		{3.5, 0.0, 3.0},   
		{4.0, 1.0, 3.5}  
	},
	{
		{4.0, 1.0, 3.5},    
		{4.5, 2.0, 4.0},   
		{5.0, 0.5, 4.5}, 
		{5.5, 1.0, 5.0}   
	}
	};

	//bezier 
	std::vector<glm::vec3> combinedCurvePoints;

	for (const auto& cps : bezierSegments) {
		BezierCurve curve(cps);
		std::vector<glm::vec3> segmentPoints = curve.GeneratePoints(100); // 100 punten per segment
		combinedCurvePoints.insert(combinedCurvePoints.end(), segmentPoints.begin(), segmentPoints.end());
	}

	// Maak een VAO voor de gecombineerde Bézier-curve
	unsigned int curveVAO = createLineVAO(combinedCurvePoints);

	Shader bezierShader(".\\BezierShader.vert", ".\\BezierShader.frag");

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

		glm::mat4 heightmapModel = glm::mat4(1.0f);
		heightmap.Render(projection, view, heightmapModel);

		// Render de Bézier-curve
		bezierShader.use();
		bezierShader.setMat4("projection", projection);
		bezierShader.setMat4("view", view);

		glm::mat4 bezierModel = glm::mat4(1.0f);
		bezierModel = glm::scale(bezierModel, glm::vec3(2.0f, 2.0f, 2.0f));
		bezierShader.setMat4("model", bezierModel);

		glLineWidth(10.0f);

		glBindVertexArray(curveVAO);
		glDrawArrays(GL_LINE_STRIP, 0, combinedCurvePoints.size());

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	// Delete buffers
	glDeleteVertexArrays(1, &curveVAO);

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

// This method returns the VAO 
// It creates a VAO and VBO for the line of the Bezier curve
unsigned int createLineVAO(const std::vector<glm::vec3>& points) {
	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), points.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return VAO;
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