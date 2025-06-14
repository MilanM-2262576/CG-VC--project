#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>

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
#include "Cannon.h"
#include "Sphere.h"
#include "ColorPicker.h"
#include "ChromaKey.h"
#include "ParticleSystem.h"
#include "Light.h"
#include "Utilities.h"
#include "SkyBox.h"
#include "PostProcessor.h"
#include "PostProcessKernel.h"

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

// particles
bool fireActive = false;

//colorpicker
ColorPicker* colorPicker = nullptr;
Sphere* redSphere = nullptr;

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

// Current kernel
PostProcessKernel::Type currentKernelType = PostProcessKernel::Type::Default;

//functions
void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
GLFWwindow* InitializeGLFW();


int main() {
	//Initialize GLFW window
	GLFWwindow* window = InitializeGLFW();
	if (!window) { return -1; }

	colorPicker = new ColorPicker(SCR_WIDTH, SCR_HEIGHT);

	PostProcessor postProcessor(SCR_WIDTH, SCR_HEIGHT, ".\\PostProcessShader.vert", ".\\PostProcessShader.frag");

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
	Cart cart(&rollerCoaster, 40.0f); 


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

	// Create canons
	std::vector<Cannon> cannons;
	std::vector<glm::vec2> cannonPositions = {
		{160, 120},
		{120, 140},
	};
	
	for (const auto& pos2d : cannonPositions) {
		float x = pos2d.x;
		float z = pos2d.y;
		float y = heightmap.GetHeightAt(x, z);
		cannons.emplace_back(glm::vec3(x, y, z), 0.04f); // Adjust scale as needed
	}

	// interactive sphere
	redSphere = new Sphere(glm::vec3(79.0f, 36.0f, 136.0f), 5.0f, glm::vec3(1.0f, 0.0f, 0.0f));

	//fire
	ParticleSystem fireParticles(100, ".\\fire.png");

	std::vector<glm::vec3> firePositionsLeft;
	std::vector<glm::vec3> firePositionsRight;

	float fireSpacing = 8.0f; 
	float halfWidth = 2.5f * 0.5f; 

	for (const auto& segment : track.GetSegments()) {
		BezierCurve curve(segment);
		std::vector<glm::vec3> points = curve.GeneratePoints(100);

		float accumulated = 0.0f;
		for (size_t i = 1; i < points.size(); ++i) {
			glm::vec3 prev = points[i - 1];
			glm::vec3 curr = points[i];
			float dist = glm::length(curr - prev);
			accumulated += dist;
			if (accumulated >= fireSpacing) {
				glm::vec3 direction = glm::normalize(curr - prev);
				glm::vec3 up(0, 1, 0);
				if (glm::length(glm::cross(direction, up)) < 0.01f)
					up = glm::vec3(1, 0, 0);
				glm::vec3 right = glm::normalize(glm::cross(up, direction));
				up = glm::normalize(glm::cross(direction, right));

				firePositionsLeft.push_back(curr - right * halfWidth);
				firePositionsRight.push_back(curr + right * halfWidth);

				accumulated = 0.0f;
			}
		}
	}

	std::vector<ParticleSystem> fireEmitters;
	for (const auto& pos : firePositionsLeft)
		fireEmitters.emplace_back(50, ".\\fire.png");
	for (const auto& pos : firePositionsRight)
		fireEmitters.emplace_back(50, ".\\fire.png");


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
		postProcessor.StartRender();

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

		// Render cannons
		for (auto& cannon : cannons) {
			cannon.Render(projection, view);
		}

		// Render interactieve vlag
		redSphere->Render(projection, view);

		//render vuur 
		for (size_t i = 0; i < firePositionsLeft.size(); ++i) {
			fireEmitters[i].SetActive(fireActive);
			fireEmitters[i].Update(deltaTime, firePositionsLeft[i]);
			fireEmitters[i].Render(projection, view);
		}
		for (size_t i = 0; i < firePositionsRight.size(); ++i) {
			fireEmitters[i + firePositionsLeft.size()].SetActive(fireActive);
			fireEmitters[i + firePositionsLeft.size()].Update(deltaTime, firePositionsRight[i]);
			fireEmitters[i + firePositionsLeft.size()].Render(projection, view);
		}

		if (camera.cameraOption == 1)
			camera.UpdateCartCamera(cart.GetPosition(), cart.GetDirection());


		// Render the heightmap
		glm::mat4 heightmapModel = glm::mat4(1.0f);
		heightmap.Render(projection, view, heightmapModel);

		// Render wat 
		water.SetTime(currentFrame);
		water.Render(projection, view);

		// Render the SkyBox
		skybox.Render(projection, view);

		PostProcessKernel selectedKernel(currentKernelType);
		postProcessor.EndRender(selectedKernel, 1.0f / 300.0f);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

		//chroma keying
		static ChromaKey chromaKey(SCR_WIDTH, SCR_HEIGHT,
			".\\models\\ChromaKeying\\dog.jpeg"
		);
		chromaKey.Render();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

	// Get primary monitor and its video mode
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	// Optionally update SCR_WIDTH and SCR_HEIGHT if they are not const
	// SCR_WIDTH = mode->width;
	// SCR_HEIGHT = mode->height;


	// Change these windows to get fullscreen or not
	//GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CG&VC project", NULL, NULL);

	GLFWwindow* window = glfwCreateWindow(
		mode->width, mode->height,
		"CG&VC project",
		monitor, // Fullscreen
		NULL
	);

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

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		int mx = SCR_WIDTH / 2;
		int my = SCR_HEIGHT / 2;

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		glm::mat4 view = camera.GetViewMatrix();


		unsigned char data[3];
		glReadPixels(mx, my, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, data);
		std::cout << "Pick color: " << (int)data[0] << "," << (int)data[1] << "," << (int)data[2] << std::endl;

		if (colorPicker->Pick(mx, my, projection, view, *redSphere)) {
			fireActive = !fireActive;
		}

	}
	
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

	if (key == GLFW_KEY_F && action == GLFW_PRESS)
		fireActive = !fireActive;

	// Change kernel type with 'K'
	if (key == GLFW_KEY_K && action == GLFW_PRESS) {
		// Cycle through kernel types
		if (currentKernelType == PostProcessKernel::Type::Gaussian)
			currentKernelType = PostProcessKernel::Type::Laplacian;
		else if (currentKernelType == PostProcessKernel::Type::Laplacian)
			currentKernelType = PostProcessKernel::Type::Default;
		else
			currentKernelType = PostProcessKernel::Type::Gaussian;

		std::cout << "Kernel changed to: " << PostProcessKernel(currentKernelType).Name() << std::endl;
	}
}

// glfw: whenever the window size changed, this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}