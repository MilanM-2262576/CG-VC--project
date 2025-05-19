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
#include "BezierTrack.h"
#include "Tree.h"
#include "Boat.h"
#include "Ship.h"
#include "Shipwreck.h"
#include "Tower.h"

#include "Light.h"
#include "Utilities.h"
#include "SkyBox.h"

#include <random>

// Screen size
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

//Camera
Camera camera(glm::vec3(0.0f, 100.0f, 3.0f)); // Pretty high for now so you can see the heightmap better
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

//functions
void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
GLFWwindow* InitializeGLFW();

int main() {
	//Initialize GLFW window
	GLFWwindow* window = InitializeGLFW();
	if (!window) { return -1; }

	std::vector<std::vector<glm::vec3>> bezierSegments = {
		// Segment 1: Start met steile klim
		{
			{-120.0f, 30.0f, -120.0f},                // Beginpunt
			{-90.0f, 35.0f, -90.0f},                  // Langzame start
			{-60.0f, 50.0f, -60.0f},                  // Steile klim
			{0.0f, 80.0f, 0.0f}                       // Hoge top (verhoogd)
		},
		// Segment 2: Steile afdaling
		{
			{0.0f, 80.0f, 0.0f},                      // Hoge top
			{20.0f, 60.0f, 20.0f},                    // Begin steile afdaling
			{40.0f, 35.0f, 40.0f},                    // Voortzetting afdaling
			{60.0f, 25.0f, 60.0f}                     // Lager eindpunt voor meer versnelling
		},
		// Segment 3: Looping omhoog
		{
			{60.0f, 25.0f, 60.0f},                    // Beginpunt laag
			{80.0f, 15.0f, 90.0f},                    // Controle voor bocht en daling
			{100.0f, 10.0f, 110.0f},                  // Laagste punt
			{120.0f, 40.0f, 120.0f}                   // Omhoog na dip
		},
		// Segment 4: Snelle bocht met banking naar rechts
		{
			{120.0f, 40.0f, 120.0f},                  // Start hoog
			{130.0f, 45.0f, 60.0f},                   // Banking naar rechts (hoger)
			{130.0f, 40.0f, 0.0f},                    // Banking houden
			{120.0f, 35.0f, -60.0f}                   // Uitkomen van bocht
		},
		// Segment 5: Kurketrekker (eerste deel)
		{
			{120.0f, 35.0f, -60.0f},                  // Start kurketrekker
			{100.0f, 50.0f, -90.0f},                  // Omhoog en draai
			{60.0f, 55.0f, -100.0f},                  // Hoogste punt kurketrekker
			{20.0f, 45.0f, -80.0f}                    // Begin afdaling
		},
		// Segment 6: Kurketrekker (tweede deel)
		{
			{20.0f, 45.0f, -80.0f},                   // Vervolg kurketrekker
			{0.0f, 35.0f, -70.0f},                    // Naar beneden draaien
			{-30.0f, 25.0f, -90.0f},                  // Laagste punt
			{-60.0f, 20.0f, -120.0f}                  // Eindpunt kurketrekker
		},
		// Segment 7: Laatste heuvels en naar start
		{
			{-60.0f, 20.0f, -120.0f},                 // Beginpunt laatste segment
			{-75.0f, 35.0f, -110.0f},                 // Kleine heuvel omhoog
			{-90.0f, 25.0f, -130.0f},                 // Kleine dip
			{-120.0f, 30.0f, -120.0f}                 // Terug bij start
		}
	};

	BezierTrack track(bezierSegments);

	// Create the rollercoaster
	RollerCoaster rollerCoaster(track.GetSegments(), 32);

	// Create a cart
	Cart cart(&rollerCoaster, 0.3f); 

	// Create Heightmap
	Heightmap heightmap(".\\heightmap.jpeg", ".\\textures", 64.0f / 256.0f, 16.0f);


	// Create Trees
	std::vector<Tree> trees;

	// Tree positions 
	std::vector<glm::vec2> treePositions = {
		{-132, 18}, {-166, -2}, {-183, -16}, {-165, -24}, {-157, -67}, {-152, -101},
		{-132, -122}, {-115, -172}, {-124, -199}, {-111, -187}, {-82, -155}, {-43, -114}, {-18, -113},
		{11, -110}, {63, -109}, {88, -113}, {107, -102},
		{128, -131}, {144, -121},  {172, -107}, {174, -92}, {172, -74}, {154, -69}, {157, -54}, {139, -31},
		{150, 36}, {142, 47}, {158, 61}, {123, 80}, {102, 81},
		{100, 36}, {107, 9}, {121, 13},
		{109, 60}, {80, 91}, {75, 111}, {49, 122}, {30, 117},
		{-10, 134}, {-23, 145},
		{-90, 78}, {-101, 88}, {-113, 122}, {-106, 146},  {-164, 100}
	};

	for (const auto& pos2d : treePositions){
		float x = pos2d.x;
		float z = pos2d.y;
		float y = heightmap.GetHeightAt(x, z);
		trees.emplace_back(glm::vec3(x, y, z), 0.1f); 
	}


	// Create boats 
	std::vector<Boat> boats;
	std::vector<glm::vec2> boatPositions = {
		{-56, 249},
		{-216, 203},
		{-263, -121},
		{-18, -260}
	};

	for (const auto& pos2d : boatPositions) {
		float x = pos2d.x;
		float z = pos2d.y;
		float y = -1.0;
		boats.emplace_back(glm::vec3(x, y, z), 0.05f); 
	}

	// Create ships
	std::vector<Ship> ships;
	std::vector<glm::vec2> shipPositions = {
		{195, 240},
		{269, -25}
	};

	for (size_t i = 0; i < shipPositions.size(); ++i) {
		float x = shipPositions[i].x;
		float z = shipPositions[i].y;
		float y = -6.0f;
		ships.emplace_back(glm::vec3(x, y, z), 0.05f);
		if (i == 0) {
			ships.back().SetRotation(glm::vec3(0.0f, glm::radians(-90.0f), 0.0f));
		}
	}
	// Create shipwrecks
	std::vector<Shipwreck> shipwrecks;
	std::vector<glm::vec2> shipwreckPositions = {
		{147, -253} // Rounded from (146.544, -253.331)
	};

	for (const auto& pos2d : shipwreckPositions) {
		float x = pos2d.x;
		float z = pos2d.y;
		float y = -8.0f; // Place it at/below water, adjust as needed
		shipwrecks.emplace_back(glm::vec3(x, y, z), 0.05f);
	}

	// Create towers
	std::vector<Tower> towers;
	std::vector<glm::vec2> towerPositions = {
		{-90, -102} 
	};

	for (const auto& pos2d : towerPositions) {
		float x = pos2d.x;
		float z = pos2d.y;
		float y = heightmap.GetHeightAt(x, z);
		towers.emplace_back(glm::vec3(x, y, z), 0.04f); // Adjust scale as needed
	}


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

		//Render Trees
		for (auto& tree : trees) {
			tree.Render(projection, view);
		}

		// Render Boats
		for (auto& boat : boats) {
			boat.Render(projection, view);
		}

		// Render Ships
		for (auto& ship : ships) {
			ship.Render(projection, view);
		}

		// Render Shipwrecks
		for (auto& shipwreck : shipwrecks) {
			shipwreck.Render(projection, view);
		}

		// Render Towers
		for (auto& tower : towers) {
			tower.Render(projection, view);
		}

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
	glfwSetMouseButtonCallback(window, mouse_button_callback);
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

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		// Print the camera's world position
		std::cout << "Camera position: ("
			<< camera.Position.x << ", "
			<< camera.Position.y << ", "
			<< camera.Position.z << ")" << std::endl;
	}
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

unsigned int loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}