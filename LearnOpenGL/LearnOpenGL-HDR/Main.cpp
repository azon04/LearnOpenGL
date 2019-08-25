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

#include "stb_image.h"

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
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), 90.0f);

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
	Shader hdrShader("Shaders/HDR/HDRShader.vs", "Shaders/HDR/HDRToneMappingShader.frag");
	Shader shader("Shaders/HDR/Simple3DShader.vs", "Shaders/HDR/Simple3DShader.frag");
	
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

	// Configure floating point FrameBuffer
	GLuint hdrFBO;
	glGenFramebuffers(1, &hdrFBO);
	// Create floating point color buffer
	GLuint colorBuffer;
	glGenTextures(1, &colorBuffer);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// create depth buffer (renderbuffer)
	GLuint rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	// attach buffer
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Setting up Textures
	GLuint diffuseMap;

	int t_width, t_height;
	unsigned char* image = stbi_load("Resources/textures/wood.png", &t_width, &t_height, 0, STBI_rgb);

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

	// Setup Lights
	// Setup Directional Light
	PointLight pointLight(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.25f, 0.25f , 0.25f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f, 0.75f, 0.05f);
	
	// Positions
	std::vector<glm::vec3> lightPositions;
	lightPositions.push_back(glm::vec3( 0.0f, 0.0f, 49.5f )); // back light
	lightPositions.push_back(glm::vec3(-1.4f, -1.9f, 9.0f));
	lightPositions.push_back(glm::vec3(0.0f, -1.8f, 4.0f));
	lightPositions.push_back(glm::vec3(0.8f, -1.7f, 6.0f));

	// Colors
	std::vector<glm::vec3> lightColors;
	lightColors.push_back(glm::vec3(200.0f, 200.0f, 200.0f));
	lightColors.push_back(glm::vec3(0.1f, 0.0f, 0.0f));
	lightColors.push_back(glm::vec3(0.0f, 0.0f, 0.2f));
	lightColors.push_back(glm::vec3(0.0f, 0.1f, 0.0f));

	// set mouse callbacks
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// set key callbacks
	glfwSetKeyCallback(window, key_callback);

	// Setup
	glEnable(GL_DEPTH_TEST);

	bool hdr = true;
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

		// 1. First render Lighted Scene to HDR Frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

		// Configure shader and matrices
		// Light Projection
		glViewport(0, 0, width, height);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw
		shader.Use();

		glm::mat4 view = camera.GetViewMatrix();

		// position of camera
		shader.setVec3("viewPos", camera.Position);

		// Setting up Material
		shader.setInt("material.diffuse", 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		shader.setInt("diffuseTexture", 0);

		// Setting up Point Light
		for (int i = 0; i < lightPositions.size(); i++)
		{
			shader.setVec3(("lights[" + std::to_string(i) + "].Position").c_str(), lightPositions[i]);
			shader.setVec3(("lights[" + std::to_string(i) + "].Color").c_str(), lightColors[i]);
		}

		glm::mat4 projection = glm::perspective(camera.Zoom, width / (float)height, 0.1f, 100.0f);

		shader.setMat4("view", view);
		shader.setMat4("projection", projection);

		glBindVertexArray(VAO_cube);

		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 25.0f));
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 27.5f));

		shader.setBool("reverse_normals", true);

		shader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		shader.setBool("reverse_normals", false);

		glBindVertexArray(0);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 2. Render HDR Frame buffer to screen
		glViewport(0, 0, width, height);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(quadVAO);

		hdrShader.Use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffer);
		hdrShader.setInt("hdrBuffer", 0);

		hdrShader.setBool("hdr", hdr);
		hdrShader.setFloat("exposure", exposure);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	// Deleting Buffer vertex array, vertex buffer and Element Buffer
	glDeleteVertexArrays(1, &VAO_cube);
	glDeleteBuffers(1, &VBO_cube);
	glDeleteRenderbuffers(1, &rboDepth);
	glDeleteTextures(1, &colorBuffer);
	glDeleteFramebuffers(1, &hdrFBO);
	glDeleteBuffers(1, &quadVBO);
	glDeleteVertexArrays(1, &quadVAO);

	// Terminate before close
	glfwTerminate();

	return 0;
}