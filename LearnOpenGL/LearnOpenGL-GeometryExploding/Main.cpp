
// GLEW

#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>

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

// Global Variables
Camera camera(0.0f, 0.0f, 5.0f, 0.0f, 1.0f, 0.0f);

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
	Shader shader("Shaders/ExplodingShader.vs", "Shaders/SimpleShaderLightColor.frag", "Shaders/ExplodingShader.gs");

	// Setup Lights
	// Setup Directional Light
	DirLight dirLight(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.5f, 0.5f, 0.5f));
	
	// Set up Point Lights
	PointLight pointLights[4] = {
		{ glm::vec3(0.7f, 0.2f, 2.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.1f, 0.1f, 0.1f), 1.0f, 0.07f, 0.017f },
		{ glm::vec3(2.3f, 3.3f, -4.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.1f, 0.1f, 0.1f), 1.0f, 0.045f, 0.0075f },
		{ glm::vec3(-4.0f, 2.0f, -12.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.1f, 0.1f, 0.1f), 1.0f, 0.022f, 0.0019f },
		{ glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.1f, 0.1f, 0.1f), 1.0f, 0.022f, 0.0019f },
	};

	// Setup SpotLight
	SpotLight spotLight(camera.Position, camera.Front, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f)));

	// Model Loading
	Model ourModel("Resources/nanosuit/nanosuit.obj");

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
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw 3D
		shader.Use();

		glm::mat4 view = camera.GetViewMatrix();

		// position of camera
		shader.setVec3("viewPos", camera.Position);
		
		shader.setFloat("material.shininess", 32.0F);

		// Setting up Directional Light
		shader.setVec3("dirLight.ambient", dirLight.Ambient);
		shader.setVec3("dirLight.diffuse", dirLight.Diffuse);
		shader.setVec3("dirLight.specular", dirLight.Specular);
		shader.setVec3("dirLight.direction", dirLight.Direction);
		
		// Setting up Point Lights
		shader.setVec3("pointLights[0].position", pointLights[0].Position);
		shader.setFloat("pointLights[0].constant", pointLights[0].Constant);
		shader.setFloat("pointLights[0].linear", pointLights[0].Linear);
		shader.setFloat("pointLights[0].quadratic", pointLights[0].Quadratic);
		shader.setVec3("pointLights[0].ambient", pointLights[0].Ambient);
		shader.setVec3("pointLights[0].diffuse", pointLights[0].Diffuse);
		shader.setVec3("pointLights[0].specular", pointLights[0].Specular);

		shader.setVec3("pointLights[1].position", pointLights[1].Position);
		shader.setFloat("pointLights[1].constant", pointLights[1].Constant);
		shader.setFloat("pointLights[1].linear", pointLights[1].Linear);
		shader.setFloat("pointLights[1].quadratic", pointLights[1].Quadratic);
		shader.setVec3("pointLights[1].ambient", pointLights[1].Ambient);
		shader.setVec3("pointLights[1].diffuse", pointLights[1].Diffuse);
		shader.setVec3("pointLights[1].specular", pointLights[1].Specular);

		shader.setVec3("pointLights[2].position", pointLights[2].Position);
		shader.setFloat("pointLights[2].constant", pointLights[2].Constant);
		shader.setFloat("pointLights[2].linear", pointLights[2].Linear);
		shader.setFloat("pointLights[2].quadratic", pointLights[2].Quadratic);
		shader.setVec3("pointLights[2].ambient", pointLights[2].Ambient);
		shader.setVec3("pointLights[2].diffuse", pointLights[2].Diffuse);
		shader.setVec3("pointLights[2].specular", pointLights[2].Specular);

		shader.setVec3("pointLights[3].position", pointLights[3].Position);
		shader.setFloat("pointLights[3].constant", pointLights[3].Constant);
		shader.setFloat("pointLights[3].linear", pointLights[3].Linear);
		shader.setFloat("pointLights[3].quadratic", pointLights[3].Quadratic);
		shader.setVec3("pointLights[3].ambient", pointLights[3].Ambient);
		shader.setVec3("pointLights[3].diffuse", pointLights[3].Diffuse);
		shader.setVec3("pointLights[3].specular", pointLights[3].Specular);

		// Setup spot light
		spotLight.Position = camera.Position;
		spotLight.Direction = camera.Front;
		shader.setVec3("spotLight.position", spotLight.Position);
		shader.setVec3("spotLight.direction", spotLight.Direction);
		shader.setFloat("spotLight.cutOff", spotLight.CutOff);
		shader.setFloat("spotLight.outerCutOff", spotLight.OuterCutOff);

		shader.setVec3("spotLight.ambient", spotLight.Ambient);
		shader.setVec3("spotLight.diffuse", spotLight.Diffuse);
		shader.setVec3("spotLight.specular", spotLight.Specular);
		shader.setFloat("spotLight.constant", spotLight.Constant);
		shader.setFloat("spotLight.linear", spotLight.Linear);
		shader.setFloat("spotLight.quadratic", spotLight.Quadratic);

		// view, projection, model
		glm::mat4 projection = glm::perspective(camera.Zoom, width / (float)height, 0.1f, 100.0f);
		
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		shader.setFloat("time", currentFrame);

		// render the loaded model
		{
			glm::mat4 model;
			model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
			model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
			shader.setMat4("model", model);
			ourModel.Draw(&shader);
		}

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	// Deleting Buffer vertex array, vertex buffer and Element Buffer
	ourModel.Release();

	ShaderManager::Destroy();
	
	// Terminate before close
	glfwTerminate();

	return 0;
}