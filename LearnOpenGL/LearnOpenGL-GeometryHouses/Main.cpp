
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

#include "SOIL.h"

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
	Shader shader("Shaders/TestGeometry/GeometryHouseVertex.vs", "Shaders/TestGeometry/GeometryHouseFragment.frag", "Shaders/TestGeometry/GeometryHouseGeoShader.gs");

	// Initialize all buffers
	float points[] = 
	{
		-0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.0f, 0.0f , 1.0f,
		-0.5f, -0.5f, 1.0f, 1.0f, 0.0f
	};
	
	GLuint VBO_points;
	glGenBuffers(1, &VBO_points);
	
	GLuint VAO_points;
	glGenVertexArrays(1, &VAO_points);

	glBindVertexArray(VAO_points);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_points);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	
	// Setting up Textures
	
	// Setup Lights
	
	// set mouse callbacks
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// set key callbacks
	glfwSetKeyCallback(window, key_callback);

	// Setup
	glEnable(GL_DEPTH_TEST);

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

		// Draw
		shader.Use();
		glBindVertexArray(VAO_points);
		
		glDrawArrays(GL_POINTS, 0, 4);

		glBindVertexArray(0);
		// Swap the buffers
		glfwSwapBuffers(window);
	}

	// Deleting All Buffers
	

	ShaderManager::Destroy();

	// Terminate before close
	glfwTerminate();

	return 0;
}