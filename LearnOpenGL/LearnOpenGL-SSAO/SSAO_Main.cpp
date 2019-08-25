#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>

#include "Shader.h"
#include "Camera.h"
#include "Material.h"
#include "PointLight.h"
#include "Model.h"

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

float lerp(float a, float b, float f)
{
	return a + f * (b - a);
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
	Shader geometryPassShader("Shaders/SSAO/GBufferShaderSSAO.vs", "Shaders/SSAO/GBufferShaderSSAO.frag");
	Shader deferredLightPassShader("Shaders/SSAO/DeferredShaderSSAO.vs", "Shaders/SSAO/DeferredShaderSSAO.frag");
 	Shader lightBoxShader("Shaders/DeferredShading/LightBoxShader.vs", "Shaders/DeferredShading/LightBoxShader.frag");
	Shader screenShader("Shaders/ScreenShader.vs", "Shaders/BaseScreenShader.frag");
	Shader ssaoShader("Shaders/SSAO/SSAOShader.vs", "Shaders/SSAO/SSAOShader.frag");
	Shader ssaoBlurShader("Shaders/SSAO/SSAOShader.vs", "Shaders/SSAO/SSAOBlurShader.frag");
	
	// Initialize all buffers
	// 3D Plane
	GLfloat plane_vertices[] = 
	{
		// positions			// normals				// texture coords
		-1.0f,  0.0f, -1.0f,	0.0f,  1.0f,  0.0f,		0.0f, 1.0f,
		1.0f,  0.0f,  1.0f,		0.0f,  1.0f,  0.0f,		1.0f, 0.0f,
		1.0f,  0.0f, -1.0f,		0.0f,  1.0f,  0.0f,		1.0f, 1.0f,
		-1.0f,  0.0f,  1.0f,	0.0f,  1.0f,  0.0f,		0.0f, 0.0f,
		1.0f,  0.0f,  1.0f,		0.0f,  1.0f,  0.0f,		1.0f, 0.0f,
		-1.0f,  0.0f, -1.0f,	0.0f,  1.0f,  0.0f,		0.0f, 1.0f
	};

	GLuint VAO_plane;
	GLuint VBO_plane;
	glGenBuffers(1, &VBO_plane);

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

	// Setting up Diffuse Map
	int t_width, t_height;
	unsigned char* image = stbi_load("Resources/textures/container2.png", &t_width, &t_height, 0, STBI_rgb);

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

	// Set up Specular map
	image = stbi_load("Resources/textures/container2_specular.png", &t_width, &t_height, 0, STBI_rgb);

	GLuint specularMap;

	glGenTextures(1, &specularMap);

	glBindTexture(GL_TEXTURE_2D, specularMap);
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	// - normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	// - color + specular color buffer
	glGenTextures(1, &gColorSpec);
	glBindTexture(GL_TEXTURE_2D, gColorSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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

	// Create SSAO Frame Buffer
	GLuint ssaoFBO;
	glGenFramebuffers(1, &ssaoFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

	GLuint ssaoColorBuffer;
	glGenTextures(1, &ssaoColorBuffer);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);

	// Create SSAO Blur Frame Buffer
	GLuint ssaoBlurFBO;
	glGenFramebuffers(1, &ssaoBlurFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);

	GLuint ssaoColorBufferBlur;
	glGenTextures(1, &ssaoColorBufferBlur);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);

	// Setup Light
	// Light attributes
	const unsigned int NR_LIGHTS = 32;
	float constant = 1.0f;
	float linear = 0.09f;
	float quadratic = 0.032f;
	glm::vec3 lightPosition(0.0f, 1.0f, 0.0f);
	glm::vec3 lightColor(0.2f, 0.2f, 0.7f);

	// Set up SSAO kernel
	std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);
	std::default_random_engine generator;
	std::vector<glm::vec3> ssaoKernel;
	for (unsigned int i = 0; i < 64; i++)
	{
		glm::vec3 sample(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator)
		);
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		float scale = (float)i / 64.0;
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		ssaoKernel.push_back(sample);
	}

	// Setup Random kernel rotations/Noise
	std::vector<glm::vec3> ssaNoise;
	for (unsigned int i = 0; i < 16; i++)
	{
		glm::vec3 noise(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			0.0f
		);
		ssaNoise.push_back(noise);
	}

	// Setup SSA Noise Texture
	GLuint noiseTexture;
	glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

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

		// 1. Geometry Pass. Note: This SSAO implemented in ViewSpace so gPosition and gNormal must be in View-Space as well
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

		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, 3.0f));
		model = glm::scale(model, glm::vec3(0.4f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		geometryPassShader.setMat4("model", model);
		ourModel.Draw(&geometryPassShader);

		// Draw Planes
		glBindVertexArray(VAO_plane);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		model = glm::mat4();
		model = glm::scale(model, glm::vec3(5.0f));
		geometryPassShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -1.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		geometryPassShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		model = glm::mat4();
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f));
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -1.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		geometryPassShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(0);

		// 2. Render SSAO
		glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		ssaoShader.Use();
		ssaoShader.setInt("gPosition", 0);
		ssaoShader.setInt("gNormal", 1);
		ssaoShader.setInt("texNoise", 2);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, noiseTexture);

		ssaoShader.setMat4("projection", projection);

		for (unsigned int i = 0; i < 64; i++)
		{
			ssaoShader.setVec3(("samples[" + std::to_string(i) + "]").c_str(), ssaoKernel[i]);
		}

		glm::vec2 noiseScale = glm::vec2(width / 4.0f, height / 4.0f);
		ssaoShader.setVec2("noiseScale", noiseScale);

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);

		// 3. SSAO Blur
		glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		ssaoBlurShader.Use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);

		// 4. Render Deferred Shading With SSAO
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
 		//glViewport(0, 0, width, height);
 		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(quadVAO);

		deferredLightPassShader.Use();
		deferredLightPassShader.setInt("gPosition", 0);
		deferredLightPassShader.setInt("gNormal", 1);
		deferredLightPassShader.setInt("gAlbedoSpec", 2);
		deferredLightPassShader.setInt("ssao", 3);

		// Because SSAO which implemented in View-Space, gPosition and gNormal also in view space
		// so the light position also need to be in View-Space (multiply by view matrix)
		deferredLightPassShader.setVec3("light.position", view * glm::vec4(lightPosition, 1.0f));
		deferredLightPassShader.setVec3("light.Color", lightColor);
		deferredLightPassShader.setFloat("light.Radius", 10.0f);
		deferredLightPassShader.setFloat("light.Linear", linear);
		deferredLightPassShader.setFloat("light.Quadratic", quadratic);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gColorSpec);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	// Deleting Buffer vertex array, vertex buffer and Element Buffer
	glDeleteVertexArrays(1, &VAO_plane);
	glDeleteBuffers(1, &VBO_plane);
	glDeleteRenderbuffers(1, &rboDepth);
	glDeleteTextures(1, &gPosition);
	glDeleteTextures(1, &gNormal);
	glDeleteTextures(1, &gColorSpec);
	glDeleteFramebuffers(1, &gBuffer);
	glDeleteBuffers(1, &quadVBO);
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteFramebuffers(1, &ssaoFBO);
	glDeleteFramebuffers(1, &ssaoBlurFBO);
	glDeleteTextures(1, &ssaoColorBuffer);
	glDeleteTextures(1, &ssaoColorBufferBlur);
	glDeleteTextures(1, &diffuseMap);
	glDeleteTextures(1, &specularMap);

	// Terminate before close
	glfwTerminate();

	return 0;
}