
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <map>

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

	// init glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK) 
	{
		glfwTerminate();
		std::cout << "Failed to init GLEW: " << glewGetErrorString(err) << std::endl;
		return -1;
	}

	// Viewport setup
	glfwGetFramebufferSize(window, &width, &height);

	// Setup Shaders
	Shader renderShader("Shaders/NormalMap/Simple3DShader.vs", "Shaders/NormalMap/LightNormalMapFragment.frag");

	// Initialize all buffers
	// Calculate tangent and bitangent
	glm::vec3 pos1(1.0f, 1.0f, 0.0f);
	glm::vec3 pos2(-1.0f, 1.0f, 0.0f);
	glm::vec3 pos3(-1.0f, -1.0f, 0.0f);
	glm::vec3 pos4(1.0f, -1.0f, 0.0f);

	glm::vec2 texCoord1(0.0f, 0.0f);
	glm::vec2 texCoord2(1.0f, 0.0f);
	glm::vec2 texCoord3(1.0f, 1.0f);
	glm::vec2 texCoord4(0.0f, 1.0f);
	
	// First triangle Pos1, Pos2, Pos3
	glm::vec3 edge1 = pos1 - pos2;
	glm::vec3 edge2 = pos1 - pos3;
	glm::vec2 deltaUV1 = texCoord1 - texCoord2;
	glm::vec2 deltaUV2 = texCoord1 - texCoord3;

	float inverseDet = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

	glm::vec3 tangent1;
	tangent1.x = inverseDet * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent1.y = inverseDet * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent1.z = inverseDet * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	tangent1 = glm::normalize(tangent1);

	glm::vec3 bitangent1;
	bitangent1.x = inverseDet * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent1.y = inverseDet * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent1.z = inverseDet * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	bitangent1 = glm::normalize(bitangent1);


	// Second triangle pos1, pos3, pos4
	edge1 = pos1 - pos3;
	edge2 = pos1 - pos4;
	deltaUV1 = texCoord1 - texCoord3;
	deltaUV2 = texCoord1 - texCoord4;

	inverseDet = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

	glm::vec3 tangent2;
	tangent2.x = inverseDet * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent2.y = inverseDet * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent2.z = inverseDet * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	tangent2 = glm::normalize(tangent2);

	glm::vec3 bitangent2;
	bitangent2.x = inverseDet * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent2.y = inverseDet * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent2.z = inverseDet * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	bitangent2 = glm::normalize(bitangent2);

	GLfloat quad_vertices[] =
	{
		// positions,				// normals			// texture coords			// tangent								// bitangent
		pos1.x, pos1.y, pos1.z,		0.0f, 0.0f, 1.0f,	texCoord1.x, texCoord1.y,	tangent1.x, tangent1.y, tangent1.z,		bitangent1.x, bitangent1.y, bitangent1.z,
		pos2.x, pos2.y, pos2.z,		0.0f, 0.0f, 1.0f,	texCoord2.x, texCoord2.y,	tangent1.x, tangent1.y, tangent1.z,		bitangent1.x, bitangent1.y, bitangent1.z,
		pos3.x, pos3.y, pos3.z,		0.0f, 0.0f, 1.0f,	texCoord3.x, texCoord3.y,	tangent1.x, tangent1.y, tangent1.z,		bitangent1.x, bitangent1.y, bitangent1.z,

		pos1.x, pos1.y, pos1.z,		0.0f, 0.0f, 1.0f,	texCoord1.x, texCoord1.y,	tangent2.x, tangent2.y, tangent2.z,		bitangent2.x, bitangent2.y, bitangent2.z,
		pos3.x, pos3.y, pos3.z,		0.0f, 0.0f, 1.0f,	texCoord3.x, texCoord3.y,	tangent2.x, tangent2.y, tangent2.z,		bitangent2.x, bitangent2.y, bitangent2.z,
		pos4.x, pos4.y, pos4.z,		0.0f, 0.0f, 1.0f,	texCoord4.x, texCoord4.y,	tangent2.x, tangent2.y, tangent2.z,		bitangent2.x, bitangent2.y, bitangent2.z
	};

	GLuint VBO_quad;
	glGenBuffers(1, &VBO_quad);

	GLuint VAO_quad;
	glGenVertexArrays(1, &VAO_quad);

	glBindVertexArray(VAO_quad);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_quad);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
	
	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// texCoord
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// Tangent
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

	// Bitangent
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(11 * sizeof(GLfloat)));
	glEnableVertexAttribArray(4);

	glBindVertexArray(0);

	// Setting up Textures
	// Load brickwall diffuse
	GLuint diffuseMap;

	int diffuse_width, diffuse_height;
	unsigned char* image = SOIL_load_image("Resources/textures/brickwall.jpg", &diffuse_width, &diffuse_height, 0, SOIL_LOAD_RGB);

	glGenTextures(1, &diffuseMap);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, diffuse_width, diffuse_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	glGenerateMipmap(GL_TEXTURE_2D);

	SOIL_free_image_data(image);

	glBindTexture(GL_TEXTURE_2D, 0);

	// Load brickwall normal map
	GLuint normalMap;

	int normal_width, normal_height;
	image = SOIL_load_image("Resources/textures/brickwall_normal.jpg", &normal_width, &normal_height, 0, SOIL_LOAD_RGB);

	glGenTextures(1, &normalMap);
	glBindTexture(GL_TEXTURE_2D, normalMap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, normal_width, normal_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	glGenerateMipmap(GL_TEXTURE_2D);

	SOIL_free_image_data(image);

	glBindTexture(GL_TEXTURE_2D, 0);

	// Setup Lights
	PointLight light(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.25f, 0.25f, 0.25f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.5f, 0.25f);
	
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

		// Rendering commands here
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw
		renderShader.Use();

		glm::mat4 projection = glm::perspective(camera.Zoom, width / (float) height, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model;
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		renderShader.setMat4("projection", projection);
		renderShader.setMat4("view", view);
		renderShader.setMat4("model", model);

		renderShader.setVec3("viewPos", camera.Position);
		renderShader.setVec3("light.position", light.Position);
		renderShader.setVec3("light.ambient", light.Ambient);
		renderShader.setVec3("light.diffuse", light.Diffuse);
		renderShader.setVec3("light.specular", light.Specular);
		renderShader.setFloat("light.constant", light.Constant);
		renderShader.setFloat("light.linear", light.Linear);
		renderShader.setFloat("light.quadratic", light.Quadratic);

		renderShader.setFloat("shininess", 16.0f);

		glBindVertexArray(VAO_quad);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		renderShader.setInt("difuseMap", 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalMap);
		renderShader.setInt("normalMap", 1);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
		
		// Swap the buffers
		glfwSwapBuffers(window);
	}

	// Deleting All Buffers
	glDeleteVertexArrays(1, &VAO_quad);
	glDeleteBuffers(1, &VBO_quad);
	glDeleteTextures(1, &diffuseMap);
	glDeleteTextures(1, &normalMap);

	// Terminate before close
	glfwTerminate();

	return 0;
}