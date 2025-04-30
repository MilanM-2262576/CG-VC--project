#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Camera.h"
#include "Shader.h"
#include "BezierCurve.h"
#include "stb_image.h"

// Screen size
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//Grass
unsigned int loadTexture(const char* path);
unsigned int createPlaneVAO();

//Bezier
unsigned int createLineVAO(const std::vector<glm::vec3>& points);

//Camera
Camera camera(glm::vec3(0.0f, 2.0f, 3.0f)); // Hoger, zodat je op het grasveld neerkijkt
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

	//Viewport instellen
	glViewport(0, 0, 800, 600);


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

	//grass ground
	unsigned int planeVAO = createPlaneVAO();
	Shader grassShader(".\\GrassShader.vert", ".\\GrassShader.frag");
	unsigned int grassTexture = loadTexture(".\\grass.jpg");

	while (!glfwWindowShouldClose(window)) {
		//Time 
		// -------------------------
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Input verwerken
		// -------------------------
		processInput(window);

		// Renderen
		// --------------------------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Projection en view matrices
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();

		// Render het gras
		grassShader.use();
		grassShader.setInt("grassTexture", 0);
		grassShader.setMat4("projection", projection);
		grassShader.setMat4("view", view);

		glm::mat4 groundModel = glm::mat4(1.0f);
		grassShader.setMat4("model", groundModel);

		glBindTexture(GL_TEXTURE_2D, grassTexture);
		glBindVertexArray(planeVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// Render de Bézier-curve
		bezierShader.use();
		bezierShader.setMat4("projection", projection);
		bezierShader.setMat4("view", view);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		bezierShader.setMat4("model", model);

		glLineWidth(10.0f);

		glBindVertexArray(curveVAO);
		glDrawArrays(GL_LINE_STRIP, 0, combinedCurvePoints.size());

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

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
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
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

// This method creates a VAO and VBO for the plane of the grass
unsigned int createPlaneVAO() {
	float planeVertices[] = {
		// posities          // texcoords
		-5.0f, 0.0f, -5.0f,  0.0f, 0.0f,
		 5.0f, 0.0f, -5.0f,  1.0f, 0.0f,
		 5.0f, 0.0f,  5.0f,  1.0f, 1.0f,
		-5.0f, 0.0f,  5.0f,  0.0f, 1.0f
	};

	unsigned int planeIndices[] = {
		0, 1, 2,
		0, 2, 3
	};

	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(planeIndices), planeIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

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