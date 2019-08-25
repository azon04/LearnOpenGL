
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>

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
Camera camera(0.0f, 2.0f, 4.0f, 0.0f, 1.0f, 0.0f);

GLfloat lastX = 400, lastY = 300;

// keep track key
bool keys[1024];

// Time tracker
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;	// Time of the last frame

int width, height;

// Resources
GLuint VAO_cube;
GLuint diffuseMap;
GLuint diffuseMap_cube;

// Cube Data
const int cubeCounts = 5;
glm::vec3 cubePositions[cubeCounts] =
{
	glm::vec3(3.0f,  -2.0f,  3.0f),
	glm::vec3(1.0f,  1.5f, 1.0f),
	glm::vec3(-1.0f,  2.0f, 2.0f),
	glm::vec3(-1.0f,  -2.0f, 2.0f),
	glm::vec3(-1.0f,  1.0f, -2.0f)
};

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

void renderScene(Shader& shader)
{
	glBindVertexArray(VAO_cube);

	glm::mat4 model;
	model = glm::scale(model, glm::vec3(15.0f));

	glDisable(GL_CULL_FACE);
	shader.setBool("reverse_normals", true);

	shader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	shader.setBool("reverse_normals", false);
	glEnable(GL_CULL_FACE);

 	glActiveTexture(GL_TEXTURE1);
 	glBindTexture(GL_TEXTURE_2D, diffuseMap_cube);
 	shader.setInt("material.diffuse", 1);

	for (int i = 0; i < cubeCounts; i++)
	{
		glm::mat4 model_cube;
		model_cube = glm::translate(model_cube, cubePositions[i]);
		shader.setMat4("model", model_cube);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	glBindVertexArray(0);
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
	Shader pointShadowDepthShader("Shaders/PointShadows/DepthShader.vs", "Shaders/PointShadows/DepthShader.frag", "Shaders/PointShadows/DepthShader.gs");
	Shader shaderWithShadow("Shaders/PointShadows/Simple3DShader.vs", "Shaders/PointShadows/Simple3DShader.frag");
	
	// Initialize all buffers
	// 3D cube
	GLfloat cube_vertices[] = 
	{
		// positions			// normals				// texture coords
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, -1.0f,		0.0f, 0.0f,
		0.5f,  0.5f, -0.5f,		0.0f, 0.0f, -1.0f,		1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,		0.0f, 0.0f, -1.0f,		1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,	0.0f, 0.0f, -1.0f,		0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,		0.0f, 0.0f, -1.0f,		1.0f, 1.0f,
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
		0.5f, -0.5f, -0.5f,		1.0f,  0.0f,  0.0f,		0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,		1.0f,  0.0f,  0.0f,		1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,		1.0f,  0.0f,  0.0f,		0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,		1.0f,  0.0f,  0.0f,		0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,		1.0f,  0.0f,  0.0f,		1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,	0.0f, -1.0f,  0.0f,		0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,		0.0f, -1.0f,  0.0f,		1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,		0.0f, -1.0f,  0.0f,		1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,		0.0f, -1.0f,  0.0f,		1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,	0.0f, -1.0f,  0.0f,		0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, -1.0f,  0.0f,		0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,	0.0f,  1.0f,  0.0f,		0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,		0.0f,  1.0f,  0.0f,		1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,		0.0f,  1.0f,  0.0f,		1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,	0.0f,  1.0f,  0.0f,		0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,		0.0f,  1.0f,  0.0f,		1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,	0.0f,  1.0f,  0.0f,		0.0f, 1.0f
	};

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

	// Depth Map : Shadow Map FBO
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	// Create Cubemap texture that is used as the framebuffer's depth buffer
	const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

	unsigned int depthCubeMap;
	glGenTextures(1, &depthCubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
	for(unsigned int i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, 
			SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Setting up Textures
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

	// Texture Container
	image = stbi_load("Resources/textures/container2.png", &t_width, &t_height, 0, STBI_rgb);
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

	// Setup Lights
	// Setup Directional Light
	PointLight pointLight(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.25f, 0.25f , 0.25f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f, 0.0f, 0.0f);
	Material mat(diffuseMap, -1, 32.0F);

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

		// Update PointLight position
		pointLight.Position.z = sin(currentFrame * 0.5f) * 3.0f;

		do_movement();

		// 1. First render to depth map
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		// Configure shader and matrices
		// Light Projection
		float aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
		float near_plane = 1.0f, far_plane = 25.0f;
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near_plane, far_plane);
		
		std::vector<glm::mat4> shadowTransforms;
		shadowTransforms.push_back(shadowProj * 
			glm::lookAt(pointLight.Position, pointLight.Position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(pointLight.Position, pointLight.Position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(pointLight.Position, pointLight.Position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(pointLight.Position, pointLight.Position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(pointLight.Position, pointLight.Position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(pointLight.Position, pointLight.Position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

		pointShadowDepthShader.Use();

		pointShadowDepthShader.setMat4("shadowMatrices[0]", shadowTransforms[0]);
		pointShadowDepthShader.setMat4("shadowMatrices[1]", shadowTransforms[1]);
		pointShadowDepthShader.setMat4("shadowMatrices[2]", shadowTransforms[2]);
		pointShadowDepthShader.setMat4("shadowMatrices[3]", shadowTransforms[3]);
		pointShadowDepthShader.setMat4("shadowMatrices[4]", shadowTransforms[4]);
		pointShadowDepthShader.setMat4("shadowMatrices[5]", shadowTransforms[5]);

		pointShadowDepthShader.setVec3("lightPos", pointLight.Position);
		pointShadowDepthShader.setFloat("far_plane", far_plane);


		glCullFace(GL_FRONT); // Generate shadow based on GL_Front cull, to prevent Peter Panning
		renderScene(pointShadowDepthShader);
		glCullFace(GL_BACK);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 2. then render scene as normal with shadow mapping (using depth map)
		glViewport(0, 0, width, height);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw
		shaderWithShadow.Use();

		glm::mat4 view = camera.GetViewMatrix();

		// position of camera
		shaderWithShadow.setVec3("viewPos", camera.Position);

		// Setting up Material
		shaderWithShadow.setInt("material.diffuse", 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
		shaderWithShadow.setInt("shadowMap", 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		shaderWithShadow.setInt("material.diffuse", 1);

		shaderWithShadow.setFloat("material.shininess", mat.Shininess);

		// Setting up Point Light
		shaderWithShadow.setVec3("light.position", pointLight.Position);
		shaderWithShadow.setVec3("light.diffuse", pointLight.Diffuse);
		shaderWithShadow.setVec3("light.specular", pointLight.Specular);
		shaderWithShadow.setVec3("light.ambient", pointLight.Ambient);
		shaderWithShadow.setFloat("light.constant", pointLight.Constant);
		shaderWithShadow.setFloat("light.linear", pointLight.Linear);
		shaderWithShadow.setFloat("light.quadratic", pointLight.Quadratic);
		shaderWithShadow.setFloat("far_plane", far_plane);

		glm::mat4 projection = glm::perspective(camera.Zoom, width / (float)height, 0.1f, 100.0f);

		shaderWithShadow.setMat4("view", view);
		shaderWithShadow.setMat4("projection", projection);

		renderScene(shaderWithShadow);

		glBindTexture(GL_TEXTURE_2D, 0);

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	// Deleting Buffer vertex array, vertex buffer and Element Buffer
	glDeleteVertexArrays(1, &VAO_cube);
	glDeleteBuffers(1, &VBO_cube);
	glDeleteTextures(1, &depthCubeMap);
	glDeleteFramebuffers(1, &depthMapFBO);

	// Terminate before close
	glfwTerminate();

	return 0;
}