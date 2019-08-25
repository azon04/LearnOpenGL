// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include <iostream>

#include "Shader.h"
#include "Camera.h"
#include "Material.h"
#include "DirLight.h"

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

// Resources
GLuint VAO_plane;
GLuint VAO_cube;
GLuint diffuseMap_cube;

const int cubeCount = 3;
glm::vec3 cubePositions[cubeCount] = 
{
	glm::vec3(0.0f,  0.5f,  1.0f),
	glm::vec3(1.0f,  1.5f, 0.0f),
	glm::vec3(-1.0f,  2.0f, 2.0f)
};

// input callback function
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) 
{
	// When user presses the escape key, we set the WindowsShouldClose property to true
	// closing the application
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
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
	if (firstMouse) 
	{
		firstMouse = false;
		lastX = (GLfloat)xpos;
		lastY = (GLfloat)ypos;
	}

	GLfloat xOffset = (GLfloat)(xpos - lastX);
	GLfloat yOffset = (GLfloat)(lastY - ypos);
	lastX = (GLfloat)xpos;
	lastY = (GLfloat)ypos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) 
{
	camera.ProcessMouseScroll((GLfloat)yoffset);
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
	glBindVertexArray(VAO_plane);

	glm::mat4 model = glm::mat4();
	model = glm::scale(model, glm::vec3(20.0f));

	shader.setMat4("model", model);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(VAO_cube);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, diffuseMap_cube);
	shader.setInt("material.diffuse", 1);

	for (int i = 0; i < cubeCount; i++)
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
	if (window == nullptr) 
	{
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
	Shader simpleDepthShader("Shaders/Shadow/SimpleDepthShader.vs", "Shaders/Shadow/SimpleDepthShader.frag");
	Shader baseScreenShader("Shaders/ScreenShader.vs", "Shaders/RenderDepthScreenShader.frag");
	Shader shaderWithShadow("Shaders/Shadow/Shadow.vs", "Shaders/Shadow/Shadow.frag");
	
	// Initialize all buffers
	// 3D cube
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

	GLuint VBO_plane;
	glGenBuffers(1, &VBO_plane);

	// Generate VAO
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

	// generate VAO
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

	// Setup quad
	float quadVertices[] =
	{
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};

	GLuint VBO_quad;
	glGenBuffers(1, &VBO_quad);

	GLuint VAO_quad;
	glGenVertexArrays(1, &VAO_quad);

	glBindVertexArray(VAO_quad);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_quad);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

	// Position Attributes
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Texture Coordinate
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	// Depth Map : Shadow Map FBO
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	// Create 2D texture that is used as the framebuffer's depth buffer
	const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Setting up Textures
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
	DirLight dirLight(glm::vec3(1.0f, -2.0f, -0.5f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.25f, 0.25f, 0.25f), glm::vec3(0.5f, 0.5f, 0.5f));

	Material mat(diffuseMap, -1, 32.0F);

	// set mouse callbacks
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// set key callbacks
	glfwSetKeyCallback(window, key_callback);

	// Setup
	glEnable(GL_DEPTH_TEST);

	bool renderDebugDepth = false;

	// Main loop of drawing
	glViewport(0, 0, width, height);
	while (!glfwWindowShouldClose(window)) 
	{
		// Check and call events
		glfwPollEvents();

		// calculate delta time
		GLfloat currentFrame = (GLfloat)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		if (keys[GLFW_KEY_SPACE])
		{
			renderDebugDepth = true;
		}
		else
		{
			renderDebugDepth = false;
		}

		do_movement();

		// 1. First render to depth map
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		// Configure shader and matrices
		// Light Projection
		float near_plane = 1.0f, far_plane = 7.5f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		glm::mat4 lightView = glm::lookAt(dirLight.Direction * -2.0f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		simpleDepthShader.Use();

		simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		glCullFace(GL_FRONT);
		renderScene(simpleDepthShader);
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
		glBindTexture(GL_TEXTURE_2D, depthMap);
		shaderWithShadow.setInt("shadowMap", 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		shaderWithShadow.setInt("material.diffuse", 1);

		shaderWithShadow.setFloat("material.shininess", mat.Shininess);

		// Setting up Directional Light
		shaderWithShadow.setVec3("dirLight.ambient", dirLight.Ambient);
		shaderWithShadow.setVec3("dirLight.diffuse", dirLight.Diffuse);
		shaderWithShadow.setVec3("dirLight.specular", dirLight.Specular);
		shaderWithShadow.setVec3("dirLight.direction", dirLight.Direction);

		glm::mat4 projection = glm::perspective(camera.Zoom, width / (float)height, 0.1f, 100.0f);

		shaderWithShadow.setMat4("view", view);
		shaderWithShadow.setMat4("projection", projection);
		shaderWithShadow.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		renderScene(shaderWithShadow);

		glBindTexture(GL_TEXTURE_2D, 0);

		if (renderDebugDepth)
		{
			baseScreenShader.Use();

			glBindVertexArray(VAO_quad);
			glDisable(GL_DEPTH_TEST);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			baseScreenShader.setInt("inTexture", 0);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glBindVertexArray(0);
			glEnable(GL_DEPTH_TEST);
		}

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	// Deleting Buffer vertex array, vertex buffer and Element Buffer
	glDeleteVertexArrays(1, &VAO_plane);
	glDeleteBuffers(1, &VBO_plane);
	glDeleteVertexArrays(1, &VAO_cube);
	glDeleteBuffers(1, &VBO_cube);
	glDeleteTextures(1, &depthMap);
	glDeleteFramebuffers(1, &depthMapFBO);

	// Terminate before close
	glfwTerminate();

	return 0;
}