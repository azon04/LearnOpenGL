
// GLEW

#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <map>

#include "ShaderManager.h"
#include "Shader.h"
#include "Camera.h"
#include "Material.h"
#include "Light.h"
#include "DirLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Model.h"

#include "stb_image.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define GLM_ENABLE_EXPERIMENTAL

#include "glm/gtx/norm.hpp"

// Global Variables
Camera camera(0.0f, 2.0f, 4.0f, 0.0f, 1.0f, 0.0f);

GLfloat lastX = 400, lastY = 300;

// keep track key
bool keys[1024];

// Time tracker
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;	// Time of the last frame

int width, height;

// input callback function
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	// When user presses the escape key, we set the WindowsShouldClose property to true
	// closing the application
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	static bool firstMouse = true;
	if (firstMouse) {
		firstMouse = false;
		lastX = xpos;
		lastY = ypos;
	}

	GLfloat xOffset = xpos - lastX;
	GLfloat yOffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}

void do_movement() {

	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(CameraMovement::FORWARD, deltaTime);

	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(CameraMovement::BACKWARD, deltaTime);

	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(CameraMovement::LEFT, deltaTime);

	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(CameraMovement::RIGHT, deltaTime);
}

int main() {

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create window
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr) {
		glfwTerminate();
		std::cout << "Failed to create GLFW window" << std::endl;
		return -1;
	}
	glfwMakeContextCurrent(window);

	// init glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		glfwTerminate();
		std::cout << "Failed to init GLEW: " << glewGetErrorString(err) << std::endl;
		return -1;
	}

	// Viewport setup
	glfwGetFramebufferSize(window, &width, &height);

	// Setup Shaders
	ShaderManager::Init();

	// 3D cube
	GLfloat plane_vertices[] = {
		// positions			// normals				// texture coords
		-1.0f,  0.0f, -1.0f,	0.0f,  1.0f,  0.0f,		0.0f, 1.0f,
		1.0f,  0.0f, -1.0f,		0.0f,  1.0f,  0.0f,		1.0f, 1.0f,
		1.0f,  0.0f,  1.0f,		0.0f,  1.0f,  0.0f,		1.0f, 0.0f,
		1.0f,  0.0f,  1.0f,		0.0f,  1.0f,  0.0f,		1.0f, 0.0f,
		-1.0f,  0.0f,  1.0f,	0.0f,  1.0f,  0.0f,		0.0f, 0.0f,
		-1.0f,  0.0f, -1.0f,	0.0f,  1.0f,  0.0f,		0.0f, 1.0f
	};

	GLuint VBO_plane;
	glGenBuffers(1, &VBO_plane);

	GLuint VAO_plane;
	glGenVertexArrays(1, &VAO_plane);

	glBindVertexArray(VAO_plane);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_plane);
	glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices), plane_vertices, GL_STATIC_DRAW);

	// Position Attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Normal Attributes
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// TexCoord attributes
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	// 3D cube
	GLfloat cube_vertices[] = {
		// positions			// normals				// texture coords
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, -1.0f,		0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,		0.0f, 0.0f, -1.0f,		1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,		0.0f, 0.0f, -1.0f,		1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,		0.0f, 0.0f, -1.0f,		1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,	0.0f, 0.0f, -1.0f,		0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, -1.0f,		0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f, 1.0f,		0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,	0.0f, 0.0f, 1.0f,		0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f, 1.0f,		0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,	-1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,	-1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,	-1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,	-1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,	-1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,	-1.0f,  0.0f,  0.0f,	1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,		1.0f,  0.0f,  0.0f,		1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,		1.0f,  0.0f,  0.0f,		1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,		1.0f,  0.0f,  0.0f,		0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,		1.0f,  0.0f,  0.0f,		0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,		1.0f,  0.0f,  0.0f,		0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,		1.0f,  0.0f,  0.0f,		1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,	0.0f, -1.0f,  0.0f,		0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,		0.0f, -1.0f,  0.0f,		1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,		0.0f, -1.0f,  0.0f,		1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,		0.0f, -1.0f,  0.0f,		1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,	0.0f, -1.0f,  0.0f,		0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, -1.0f,  0.0f,		0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,	0.0f,  1.0f,  0.0f,		0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,		0.0f,  1.0f,  0.0f,		1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,		0.0f,  1.0f,  0.0f,		1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,		0.0f,  1.0f,  0.0f,		1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,	0.0f,  1.0f,  0.0f,		0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,	0.0f,  1.0f,  0.0f,		0.0f, 1.0f
	};

	GLuint VBO_cube;
	glGenBuffers(1, &VBO_cube);

	GLuint VAO_cube;
	glGenVertexArrays(1, &VAO_cube);

	glBindVertexArray(VAO_cube);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

	// Position Attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Normal Attributes
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// TexCoord attributes
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	// Vegetation Plane
	GLfloat vegetation_vertices[] = {
		// positions			// normals				// texture coords
		-1.0f,  1.0f, 0.0f,		0.0f,  1.0f,  0.0f,		0.0f, 1.0f,
		1.0f,  1.0f, 0.0f,		0.0f,  1.0f,  0.0f,		1.0f, 1.0f,
		1.0f,  -1.0f,  0.0f,	0.0f,  1.0f,  0.0f,		1.0f, 0.0f,
		1.0f,  -1.0f,  0.0f,	0.0f,  1.0f,  0.0f,		1.0f, 0.0f,
		-1.0f,  -1.0f,  0.0f,	0.0f,  1.0f,  0.0f,		0.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,		0.0f,  1.0f,  0.0f,		0.0f, 1.0f
	};

	GLuint VBO_vegetation;
	glGenBuffers(1, &VBO_vegetation);

	GLuint VAO_vegetation;
	glGenVertexArrays(1, &VAO_vegetation);

	glBindVertexArray(VAO_vegetation);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vegetation);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vegetation_vertices), vegetation_vertices, GL_STATIC_DRAW);
	
	// Position Attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Normal Attributes
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	//glEnableVertexAttribArray(1);

	// TexCoord attributes
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);


	// setting up Textures
	int t_width, t_height;
	unsigned char* image = stbi_load("Resources/textures/wall.jpg", &t_width, &t_height, 0, STBI_rgb);

	GLuint diffuseMap;

	glGenTextures(1, &diffuseMap);

	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	// Set Texture Parameters
	// Set Wrap function
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	// Set Filtering function
	// Set Mipmaping functions
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t_width, t_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(image);

	glBindTexture(GL_TEXTURE_2D, 0);

	// Texture Container
	image = stbi_load("Resources/textures/container2.png", &t_width, &t_height, 0, STBI_rgb);

	GLuint diffuseMap_cube;

	glGenTextures(1, &diffuseMap_cube);

	glBindTexture(GL_TEXTURE_2D, diffuseMap_cube);
	// Set Texture Parameters
	// Set Wrap function
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	// Set Filtering function
	// Set Mipmaping functions
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t_width, t_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(image);

	glBindTexture(GL_TEXTURE_2D, 0);

	// Texture Transparent
#if 1
	image = stbi_load("Resources/textures/blending_transparent_window.png", &t_width, &t_height, 0, STBI_rgb_alpha);
#else
	image = stbi_load("Resources/textures/grass.png", &t_width, &t_height, 0, STBI_rgb_alpha);
#endif

	GLuint diffuseMap_transparent;

	glGenTextures(1, &diffuseMap_transparent);

	glBindTexture(GL_TEXTURE_2D, diffuseMap_transparent);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t_width, t_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(image);

	glBindTexture(GL_TEXTURE_2D, 0);

	// Setup light VAO
	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);

	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_plane);

	// Position attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.5f,  2.0f),
		glm::vec3(1.0f,  0.5, 0.0f)
	};

	glm::vec3 transparentPositions[5] = {
		glm::vec3(-1.5f,  0.5f, -0.48f),
		glm::vec3(1.5f,  0.5f,  0.51f),
		glm::vec3(0.0f,  0.5f,  0.7f),
		glm::vec3(-0.3f,  0.5f, -2.3f),
		glm::vec3(0.5f,  0.5f, -0.6f)
	};
	
	glm::mat4 projection;

	// Setup Lights
	// Setup Directional Light
	DirLight dirLight(glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.25f, 0.25f, 0.25f), glm::vec3(0.5f, 0.5f, 0.5f));

	// Set up Point Lights
	PointLight pointLights[4] = {
		{ glm::vec3(0.7f, 0.2f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 0.07f, 0.017f },
		{ glm::vec3(2.3f, 3.3f, -4.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 0.045f, 0.0075f },
		{ glm::vec3(-4.0f, 2.0f, -12.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 0.022f, 0.0019f },
		{ glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 0.022f, 0.0019f },
	};

	// Setup SpotLight
	SpotLight spotLight(camera.Position, camera.Front, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 0.09f, 0.032f, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f)));

	// Material
#if USING_DIFFUSE_MAP
	Material mat(diffuseMap, -1, 32.0F);
#else
	Material mat(glm::vec3(1.0f, 0.5f, 0.31f), glm::vec3(1.0f, 0.5f, 0.31f), glm::vec3(0.5f, 0.5f, 0.5f), 32.0f);
#endif

	// set mouse callbacks
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// set key callbacks
	glfwSetKeyCallback(window, key_callback);

	// Setup
	glEnable(GL_DEPTH_TEST);

	// Enable Blending
	glEnable(GL_BLEND);
#if 0
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#else
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
#endif

	// Main loop of drawing
	glViewport(0, 0, width, height);
	while (!glfwWindowShouldClose(window)) {
		// Check and call events
		glfwPollEvents();


		// calculate delta time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		do_movement();

		// Rendering commands here
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw 3D
		Shader* shader = ShaderManager::getInstance()->getShaderByType(SHADER_TYPE_VERTICE_LIGHT_PHONG);
		shader->Use();

		glm::mat4 view = camera.GetViewMatrix();

		// position of camera
		shader->setVec3("viewPos", camera.Position);

		// Setting up Material
		shader->setInt("material.diffuse", 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);

		shader->setFloat("material.shininess", mat.Shininess);

		// Setting up Directional Light
		shader->setBool("useDirLight", true);
		shader->setVec3("dirLight.ambient", dirLight.Ambient);
		shader->setVec3("dirLight.diffuse", dirLight.Diffuse);
		shader->setVec3("dirLight.specular", dirLight.Specular);
		shader->setVec3("dirLight.direction", dirLight.Direction);

		// Setting up Point Lights
		shader->setInt("pointLightCount", 4);
		shader->setVec3("pointLights[0].position", pointLights[0].Position);
		shader->setFloat("pointLights[0].constant", pointLights[0].Constant);
		shader->setFloat("pointLights[0].linear", pointLights[0].Linear);
		shader->setFloat("pointLights[0].quadratic", pointLights[0].Quadratic);
		shader->setVec3("pointLights[0].ambient", pointLights[0].Ambient);
		shader->setVec3("pointLights[0].diffuse", pointLights[0].Diffuse);
		shader->setVec3("pointLights[0].specular", pointLights[0].Specular);

		shader->setVec3("pointLights[1].position", pointLights[1].Position);
		shader->setFloat("pointLights[1].constant", pointLights[1].Constant);
		shader->setFloat("pointLights[1].linear", pointLights[1].Linear);
		shader->setFloat("pointLights[1].quadratic", pointLights[1].Quadratic);
		shader->setVec3("pointLights[1].ambient", pointLights[1].Ambient);
		shader->setVec3("pointLights[1].diffuse", pointLights[1].Diffuse);
		shader->setVec3("pointLights[1].specular", pointLights[1].Specular);

		shader->setVec3("pointLights[2].position", pointLights[2].Position);
		shader->setFloat("pointLights[2].constant", pointLights[2].Constant);
		shader->setFloat("pointLights[2].linear", pointLights[2].Linear);
		shader->setFloat("pointLights[2].quadratic", pointLights[2].Quadratic);
		shader->setVec3("pointLights[2].ambient", pointLights[2].Ambient);
		shader->setVec3("pointLights[2].diffuse", pointLights[2].Diffuse);
		shader->setVec3("pointLights[2].specular", pointLights[2].Specular);

		shader->setVec3("pointLights[3].position", pointLights[3].Position);
		shader->setFloat("pointLights[3].constant", pointLights[3].Constant);
		shader->setFloat("pointLights[3].linear", pointLights[3].Linear);
		shader->setFloat("pointLights[3].quadratic", pointLights[3].Quadratic);
		shader->setVec3("pointLights[3].ambient", pointLights[3].Ambient);
		shader->setVec3("pointLights[3].diffuse", pointLights[3].Diffuse);
		shader->setVec3("pointLights[3].specular", pointLights[3].Specular);

		// Setup spot light
		spotLight.Position = camera.Position;
		spotLight.Direction = camera.Front;
		shader->setBool("useSpotLight", true);
		shader->setVec3("spotLight.position", spotLight.Position);
		shader->setVec3("spotLight.direction", spotLight.Direction);
		shader->setFloat("spotLight.cutOff", spotLight.CutOff);
		shader->setFloat("spotLight.outerCutOff", spotLight.OuterCutOff);

		shader->setVec3("spotLight.ambient", spotLight.Ambient);
		shader->setVec3("spotLight.diffuse", spotLight.Diffuse);
		shader->setVec3("spotLight.specular", spotLight.Specular);
		shader->setFloat("spotLight.constant", spotLight.Constant);
		shader->setFloat("spotLight.linear", spotLight.Linear);
		shader->setFloat("spotLight.quadratic", spotLight.Quadratic);

		// view, projection, model
		projection = glm::perspective(camera.Zoom, width / (float)height, 0.1f, 100.0f);
		
		shader->setMat4("view", view);
		shader->setMat4("projection", projection);

		glBindVertexArray(VAO_plane);

		glm::mat4 model;
		model = glm::scale(model, glm::vec3(5.0f));

		shader->setMat4("model", model);
		
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		glBindVertexArray(VAO_cube);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap_cube);

		for (int i = 0; i < 2; i++)
		{
			glm::mat4 model_cube;
			model_cube = glm::translate(model_cube, cubePositions[i]);
			shader->setMat4("model", model_cube);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glBindVertexArray(0);

#if USING_DIFFUSE_MAP
		glBindTexture(GL_TEXTURE_2D, 0);
#endif
		// Sort Transparent objects
		std::map<float, glm::vec3> sorted;
		for (int i = 0; i < 5; i++)
		{
			float distance = glm::length2(camera.Position - transparentPositions[i]);
			sorted[distance] = transparentPositions[i];
		}

		// Draw Transparent objects
		Shader* transparentShader = ShaderManager::getInstance()->getShaderByType(SHADER_TYPE_VERTICE_TRANSPARENT_COLOR);
		
		transparentShader->Use();

		transparentShader->setMat4("view", view);
		transparentShader->setMat4("projection", projection);

		transparentShader->setInt("texture1", 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap_transparent);

		glBindVertexArray(VAO_vegetation);

		for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
		{
			model = glm::mat4();
			model = glm::translate(model, it->second);
			model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));

			transparentShader->setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	// Deleting Buffer vertex array, vertex buffer and Element Buffer
	glDeleteVertexArrays(1, &VAO_plane);
	glDeleteBuffers(1, &VBO_plane); 
	glDeleteVertexArrays(1, &VAO_cube);
	glDeleteBuffers(1, &VBO_cube); 
	glDeleteVertexArrays(1, &VAO_vegetation);
	glDeleteBuffers(1, &VBO_vegetation);

	ShaderManager::Destroy();

	// Terminate before close
	glfwTerminate();

	return 0;
}