#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "Shader.h"
#include "Camera.h"
#include "Material.h"
#include "PointLight.h"
#include "Model.h"

#include "SOIL.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define GLM_ENABLE_EXPERIMENTAL

#include "glm/gtx/norm.hpp"

// TODO for NVIDIA Optimus :  This enable the program to use NVIDIA instead of integrated Intel graphics
#if WIN32 || WIN64
extern "C" {
#include <windows.h>
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
#endif

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
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) 
{
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

void mouse_callback(GLFWwindow* window, double xpos, double ypos) 
{
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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) 
{
	camera.ProcessMouseScroll(yoffset);
}

void do_movement() 
{
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(CameraMovement::FORWARD, deltaTime);

	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(CameraMovement::BACKWARD, deltaTime);

	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(CameraMovement::LEFT, deltaTime);

	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(CameraMovement::RIGHT, deltaTime);
}

int main() 
{
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

	// init GLEW
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
	Shader geometryPassShader("Shaders/DeferredShading/GBufferShader.vs", "Shaders/DeferredShading/GBufferShader.frag");
	Shader deferredLightPassShader("Shaders/DeferredShading/DeferredShader.vs", "Shaders/DeferredShading/DeferredShader.frag");
 	Shader lightBoxShader("Shaders/DeferredShading/LightBoxShader.vs", "Shaders/DeferredShading/LightBoxShader.frag");
	Shader screenShader("Shaders/ScreenShader.vs", "Shaders/BaseScreenShader.frag");
	
	// Initialize all buffers
	// 3D cube
	GLfloat cube_vertices[] = 
	{
		// positions			// normals				// texture coords
		-1.0f, -1.0f, -1.0f,	0.0f, 0.0f, -1.0f,		0.0f, 0.0f,
		1.0f,  1.0f, -1.0f,		0.0f, 0.0f, -1.0f,		1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,		0.0f, 0.0f, -1.0f,		1.0f, 0.0f,
		-1.0f,  1.0f, -1.0f,	0.0f, 0.0f, -1.0f,		0.0f, 1.0f,
		1.0f,  1.0f, -1.0f,		0.0f, 0.0f, -1.0f,		1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,	0.0f, 0.0f, -1.0f,		0.0f, 0.0f,

		-1.0f, -1.0f,  1.0f,	0.0f, 0.0f, 1.0f,		0.0f, 0.0f,
		1.0f, -1.0f,  1.0f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f,
		1.0f,  1.0f,  1.0f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
		1.0f,  1.0f,  1.0f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
		-1.0f,  1.0f,  1.0f,	0.0f, 0.0f, 1.0f,		0.0f, 1.0f,
		-1.0f, -1.0f,  1.0f,	0.0f, 0.0f, 1.0f,		0.0f, 0.0f,

		-1.0f,  1.0f,  1.0f,	-1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
		-1.0f,  1.0f, -1.0f,	-1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,	-1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,	-1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
		-1.0f, -1.0f,  1.0f,	-1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
		-1.0f,  1.0f,  1.0f,	-1.0f,  0.0f,  0.0f,	1.0f, 0.0f,

		1.0f,  1.0f,  1.0f,		1.0f,  0.0f,  0.0f,		1.0f, 0.0f,
		1.0f, -1.0f, -1.0f,		1.0f,  0.0f,  0.0f,		0.0f, 1.0f,
		1.0f,  1.0f, -1.0f,		1.0f,  0.0f,  0.0f,		1.0f, 1.0f,
		1.0f, -1.0f,  1.0f,		1.0f,  0.0f,  0.0f,		0.0f, 0.0f,
		1.0f, -1.0f, -1.0f,		1.0f,  0.0f,  0.0f,		0.0f, 1.0f,
		1.0f,  1.0f,  1.0f,		1.0f,  0.0f,  0.0f,		1.0f, 0.0f,

		-1.0f, -1.0f, -1.0f,	0.0f, -1.0f,  0.0f,		0.0f, 1.0f,
		1.0f, -1.0f, -1.0f,		0.0f, -1.0f,  0.0f,		1.0f, 1.0f,
		1.0f, -1.0f,  1.0f,		0.0f, -1.0f,  0.0f,		1.0f, 0.0f,
		1.0f, -1.0f,  1.0f,		0.0f, -1.0f,  0.0f,		1.0f, 0.0f,
		-1.0f, -1.0f,  1.0f,	0.0f, -1.0f,  0.0f,		0.0f, 0.0f,
		-1.0f, -1.0f, -1.0f,	0.0f, -1.0f,  0.0f,		0.0f, 1.0f,

		-1.0f,  1.0f, -1.0f,	0.0f,  1.0f,  0.0f,		0.0f, 1.0f,
		1.0f,  1.0f,  1.0f,		0.0f,  1.0f,  0.0f,		1.0f, 0.0f,
		1.0f,  1.0f, -1.0f,		0.0f,  1.0f,  0.0f,		1.0f, 1.0f,
		-1.0f,  1.0f,  1.0f,	0.0f,  1.0f,  0.0f,		0.0f, 0.0f,
		1.0f,  1.0f,  1.0f,		0.0f,  1.0f,  0.0f,		1.0f, 0.0f,
		-1.0f,  1.0f, -1.0f,	0.0f,  1.0f,  0.0f,		0.0f, 1.0f
	};

	GLuint VAO_cube;
	GLuint VBO_cube;
	glGenBuffers(1, &VBO_cube);

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

	// Quad buffer
	GLfloat quadVertices[] =
	{
		// Positions		// texture coords
		-1.0f, 1.0f, 0.0f,	0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,	1.0f, 1.0f,
		1.0f, -1.0f, 0.0f,	1.0f, 0.0f
	};

	// Load Model
	Model ourModel("Resources/nanosuit/nanosuit.obj");

	// Setup quad VAO
	GLuint quadVAO;
	GLuint quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (GLvoid*)(sizeof(GLfloat) * 3));
	glBindVertexArray(0);

	// Configure GBuffer
	GLuint gBuffer;
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	GLuint gPosition, gNormal, gColorSpec;

	// - position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	// - normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	// - color + specular color buffer
	glGenTextures(1, &gColorSpec);
	glBindTexture(GL_TEXTURE_2D, gColorSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gColorSpec, 0);

	// create depth buffer (renderbuffer)
	GLuint rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0 , GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Setup Lights
	// Light attributes
	const unsigned int NR_LIGHTS = 32;
	float constant = 1.0f;
	float linear = 0.7f;
	float quadratic = 0.5f;
	std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightColors;
	std::vector<float> lightRadius;
	srand(13);
	for (unsigned int i = 0; i < NR_LIGHTS; i++)
	{
		// calculate slightly random effects
		float xPos = ((rand() % 100) / 100.0) * 8.0 - 4.0;
		float yPos = ((rand() % 100) / 100.0) * 6.0;
		float zPos = ((rand() % 100) / 100.0) * 8.0 - 4.0;
		lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
		// also calculate random color
		float rColor = ((rand() % 100) / 200.0f) + 0.5f;
		float gColor = ((rand() % 100) / 200.0f) + 0.5f;
		float bColor = ((rand() % 100) / 200.0f) + 0.5f;
		lightColors.push_back(glm::vec3(rColor, gColor, bColor));

		// Calculate radius of lights
		float lightMax = std::fmaxf(std::fmaxf(rColor, gColor), bColor);
		float radius = (-linear + std::sqrtf(linear * linear - 4 * quadratic * (constant - (256.0 / 5.0) * lightMax))) / (2 * quadratic);
		lightRadius.push_back(radius);
	}

	// Model Positions
	std::vector<glm::vec3> modelPositions;
	modelPositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	modelPositions.push_back(glm::vec3(4.0f, 0.0f, 0.0f));
	modelPositions.push_back(glm::vec3(-4.0f, 0.0f, 0.0f));
	modelPositions.push_back(glm::vec3(0.0f, 0.0f, 4.0f));
	modelPositions.push_back(glm::vec3(4.0f, 0.0f, 4.0f));
	modelPositions.push_back(glm::vec3(-4.0f, 0.0f, 4.0f));
	modelPositions.push_back(glm::vec3(0.0f, 0.0f, -4.0f));
	modelPositions.push_back(glm::vec3(4.0f, 0.0f, -4.0f));
	modelPositions.push_back(glm::vec3(-4.0f, 0.0f, -4.0f));

	// set mouse callbacks
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// set key callbacks
	glfwSetKeyCallback(window, key_callback);

	// Setup
	glEnable(GL_DEPTH_TEST);

	bool drawLight = true;
	float exposure = 1.0f; // higher: focus on dark area; lower: focus on bright area

	// Main loop of drawing
	glViewport(0, 0, width, height);
	while (!glfwWindowShouldClose(window)) 
	{
		// Check and call events
		glfwPollEvents();

		// calculate delta time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		do_movement();

		if (keys['L'])
		{
			drawLight = true;
		}
		
		if (keys['N'])
		{
			drawLight = false;
		}

		// 1. First render Lighted Scene to HDR Frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

		// Configure shader and matrices
		// Light Projection
		glViewport(0, 0, width, height);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(camera.Zoom, width / (float)height, 0.1f, 100.0f);

		// Draw Objects
		geometryPassShader.Use();
		geometryPassShader.setMat4("view", view);
		geometryPassShader.setMat4("projection", projection);

		for(int i=0; i < modelPositions.size(); i++)
		{ 
			glm::mat4 model;
			model = glm::translate(model, modelPositions[i]);
			model = glm::scale(model, glm::vec3(0.4f));

			geometryPassShader.setMat4("model", model);
			ourModel.Draw(&geometryPassShader);
		}

		// Render Deferred Shading
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
 		//glViewport(0, 0, width, height);
 		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(quadVAO);

		deferredLightPassShader.Use();
		deferredLightPassShader.setVec3("viewPos", camera.Position);
		deferredLightPassShader.setInt("gPosition", 0);
		deferredLightPassShader.setInt("gNormal", 1);
		deferredLightPassShader.setInt("gAlbedoSpec", 2);

		for (unsigned int i = 0; i < NR_LIGHTS; i++)
		{
			deferredLightPassShader.setVec3(("lights[" + std::to_string(i) + "].Position").c_str(), lightPositions[i]);
			deferredLightPassShader.setVec3(("lights[" + std::to_string(i) + "].Color").c_str(), lightColors[i]);
			deferredLightPassShader.setFloat(("lights[" + std::to_string(i) + "].Radius").c_str(), lightRadius[i]);
			deferredLightPassShader.setFloat(("lights[" + std::to_string(i) + "].Linear").c_str(), linear);
			deferredLightPassShader.setFloat(("lights[" + std::to_string(i) + "].Quadratic").c_str(), quadratic);
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gColorSpec);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);

		if (drawLight)
		{
			glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Write to default framebuffer
			glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// Draw Lights
			lightBoxShader.Use();
			lightBoxShader.setMat4("view", view);
			lightBoxShader.setMat4("projection", projection);

			glBindVertexArray(VAO_cube);
			for (int i = 0; i < NR_LIGHTS; i++)
			{
				glm::mat4 model;
				model = glm::translate(model, lightPositions[i]);
				model = glm::scale(model, glm::vec3(0.25f));
				lightBoxShader.setMat4("model", model);
				lightBoxShader.setVec3("lightColor", lightColors[i]);

				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
			glBindVertexArray(0);
		}

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	// Deleting Buffer vertex array, vertex buffer and Element Buffer
	glDeleteVertexArrays(1, &VAO_cube);
	glDeleteBuffers(1, &VBO_cube);
	glDeleteRenderbuffers(1, &rboDepth);
	glDeleteTextures(1, &gPosition);
	glDeleteTextures(1, &gNormal);
	glDeleteTextures(1, &gColorSpec);
	glDeleteFramebuffers(1, &gBuffer);
	glDeleteBuffers(1, &quadVBO);
	glDeleteVertexArrays(1, &quadVAO);

	// Terminate before close
	glfwTerminate();

	return 0;
}