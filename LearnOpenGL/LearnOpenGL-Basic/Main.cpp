
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

	GLuint VBO;
	glGenBuffers(1, &VBO);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
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


	// setting up Textures

	// Setting up Diffuse Map
	int t_width, t_height;
	unsigned char* image = SOIL_load_image("Resources/textures/container2.png", &t_width, &t_height, 0, SOIL_LOAD_RGB);

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
	
	SOIL_free_image_data(image);

	glBindTexture(GL_TEXTURE_2D, 0);

	// Setting up Specular map
	int s_width, s_height;
	unsigned char* specular_image = SOIL_load_image("Resources/textures/container2_specular.png", &s_width, &s_height, 0, SOIL_LOAD_RGB);

	GLuint specularMap;
	
	glGenTextures(1, &specularMap);
	glBindTexture(GL_TEXTURE_2D, specularMap);
	// Set Texture Parameters
	// Set Wrap Function
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	// Set Filtering function
	// set Mipmaping function
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, s_width, s_height, 0, GL_RGB, GL_UNSIGNED_BYTE, specular_image);
	
	glGenerateMipmap(GL_TEXTURE_2D);

	SOIL_free_image_data(specular_image);
	
	glBindTexture(GL_TEXTURE_2D, 0);

	// Setup light VAO
	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);

	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Position attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	glm::mat4 trans;
	trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
	trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));

	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.0f), width / (float)height, 0.1f, 100.0f);
	
	// Setup Lights
	// Setup Directional Light
	DirLight dirLight(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.5f, 0.5f, 0.5f));
	
	// Set up Point Lights
	PointLight pointLights[4] = {
		{ glm::vec3(0.7f, 0.2f, 2.0f), glm::vec3(0.0f, 0.75f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.1f, 0.1f, 0.1f), 1.0f, 0.07f, 0.017f },
		{ glm::vec3(2.3f, 3.3f, -4.0f), glm::vec3(0.0f, 0.75f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.1f, 0.1f, 0.1f), 1.0f, 0.045f, 0.0075f },
		{ glm::vec3(-4.0f, 2.0f, -12.0f), glm::vec3(0.0f, 0.75f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.1f, 0.1f, 0.1f), 1.0f, 0.022f, 0.0019f },
		{ glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 0.75f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.1f, 0.1f, 0.1f), 1.0f, 0.022f, 0.0019f },
	};

	// Setup SpotLight
	SpotLight spotLight(camera.Position, camera.Front, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f)));

	// Material
#if USING_DIFFUSE_MAP
	Material mat(diffuseMap, specularMap, 32.0F);
#else
	Material mat(glm::vec3(1.0f, 0.5f, 0.31f), glm::vec3(1.0f, 0.5f, 0.31f), glm::vec3(0.5f, 0.5f, 0.5f), 32.0f);
#endif

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
		Shader* _3dShader = ShaderManager::getInstance()->getShaderByType(SHADER_TYPE_VERTICE_LIGHT_PHONG);
		_3dShader->Use();

		glm::mat4 view = camera.GetViewMatrix();

		// position of camera
		_3dShader->setVec3("viewPos", camera.Position);

		// Setting up Material
#if USING_DIFFUSE_MAP
		_3dShader->setInt("material.diffuse", 0);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);

		_3dShader->setInt("material.specular", 1);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
#else
		GLint matAmbientLoc = _3dShader->getUniformPosition("material.ambient");
		GLint matDiffuseLoc = _3dShader->getUniformPosition("material.diffuse");
		GLint matSpecularLoc = _3dShader->getUniformPosition("material.specular");
		glUniform3f(matAmbientLoc, mat.Ambient.r, mat.Ambient.g, mat.Ambient.b);
		glUniform3f(matDiffuseLoc, mat.Diffuse.r, mat.Diffuse.g, mat.Diffuse.b);
		glUniform3f(matSpecularLoc, mat.Specular.r, mat.Specular.g, mat.Specular.b);
		
#endif
		_3dShader->setFloat("material.shininess", mat.Shininess);
		
		// Setting up Directional Light
		_3dShader->setVec3("dirLight.ambient", dirLight.Ambient);
		_3dShader->setVec3("dirLight.diffuse", dirLight.Diffuse);
		_3dShader->setVec3("dirLight.specular", dirLight.Specular);
		_3dShader->setVec3("dirLight.direction", dirLight.Direction);
		
		// Setting up Point Lights
		_3dShader->setVec3("pointLights[0].position", pointLights[0].Position);
		_3dShader->setFloat("pointLights[0].constant", pointLights[0].Constant);
		_3dShader->setFloat("pointLights[0].linear", pointLights[0].Linear);
		_3dShader->setFloat("pointLights[0].quadratic", pointLights[0].Quadratic);
		_3dShader->setVec3("pointLights[0].ambient", pointLights[0].Ambient);
		_3dShader->setVec3("pointLights[0].diffuse", pointLights[0].Diffuse);
		_3dShader->setVec3("pointLights[0].specular", pointLights[0].Specular);

		_3dShader->setVec3("pointLights[1].position", pointLights[1].Position);
		_3dShader->setFloat("pointLights[1].constant", pointLights[1].Constant);
		_3dShader->setFloat("pointLights[1].linear", pointLights[1].Linear);
		_3dShader->setFloat("pointLights[1].quadratic", pointLights[1].Quadratic);
		_3dShader->setVec3("pointLights[1].ambient", pointLights[1].Ambient);
		_3dShader->setVec3("pointLights[1].diffuse", pointLights[1].Diffuse);
		_3dShader->setVec3("pointLights[1].specular", pointLights[1].Specular);

		_3dShader->setVec3("pointLights[2].position", pointLights[2].Position);
		_3dShader->setFloat("pointLights[2].constant", pointLights[2].Constant);
		_3dShader->setFloat("pointLights[2].linear", pointLights[2].Linear);
		_3dShader->setFloat("pointLights[2].quadratic", pointLights[2].Quadratic);
		_3dShader->setVec3("pointLights[2].ambient", pointLights[2].Ambient);
		_3dShader->setVec3("pointLights[2].diffuse", pointLights[2].Diffuse);
		_3dShader->setVec3("pointLights[2].specular", pointLights[2].Specular);

		_3dShader->setVec3("pointLights[3].position", pointLights[3].Position);
		_3dShader->setFloat("pointLights[3].constant", pointLights[3].Constant);
		_3dShader->setFloat("pointLights[3].linear", pointLights[3].Linear);
		_3dShader->setFloat("pointLights[3].quadratic", pointLights[3].Quadratic);
		_3dShader->setVec3("pointLights[3].ambient", pointLights[3].Ambient);
		_3dShader->setVec3("pointLights[3].diffuse", pointLights[3].Diffuse);
		_3dShader->setVec3("pointLights[3].specular", pointLights[3].Specular);

		// Setup spot light
		spotLight.Position = camera.Position;
		spotLight.Direction = camera.Front;
		_3dShader->setVec3("spotLight.position", spotLight.Position);
		_3dShader->setVec3("spotLight.direction", spotLight.Direction);
		_3dShader->setFloat("spotLight.cutOff", spotLight.CutOff);
		_3dShader->setFloat("spotLight.outerCutOff", spotLight.OuterCutOff);

		_3dShader->setVec3("spotLight.ambient", spotLight.Ambient);
		_3dShader->setVec3("spotLight.diffuse", spotLight.Diffuse);
		_3dShader->setVec3("spotLight.specular", spotLight.Specular);
		_3dShader->setFloat("spotLight.constant", spotLight.Constant);
		_3dShader->setFloat("spotLight.linear", spotLight.Linear);
		_3dShader->setFloat("spotLight.quadratic", spotLight.Quadratic);

		// view, projection, model
		projection = glm::perspective(camera.Zoom, width / (float)height, 0.1f, 100.0f);
		
		_3dShader->setMat4("view", view);
		_3dShader->setMat4("projection", projection);

		glBindVertexArray(VAO);

		for (int i = 0; i < 10; i++) 
		{
			glm::mat4 model;
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			
			_3dShader->setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);

		// render the loaded model
		{
			glm::mat4 model;
			model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
			model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
			_3dShader->setMat4("model", model);
			ourModel.Draw(_3dShader);
		}
		
#if USING_DIFFUSE_MAP
		glBindTexture(GL_TEXTURE_2D, 0);
#endif
		// Drawing Light
		Shader* lightShader = ShaderManager::getInstance()->getShaderByType(SHADER_TYPE_VERTICE_LIGHT_REP);
		lightShader->Use();

		lightShader->setMat4("view", view);
		lightShader->setMat4("projection", projection);

		// Setup model
		glBindVertexArray(lightVAO);
		for (int i = 0; i < 4; i++)
		{
			glm::mat4 model = glm::mat4();
			model = glm::translate(model, pointLights[i].Position);
			model = glm::scale(model, glm::vec3(0.2f));
			lightShader->setMat4("model", model);
			lightShader->setVec3("diffuse", pointLights[i].Diffuse);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	// Deleting Buffer vertex array, vertex buffer and Element Buffer
	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);

	ShaderManager::Destroy();
	
	// Terminate before close
	glfwTerminate();

	return 0;
}