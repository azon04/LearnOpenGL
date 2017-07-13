#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h> // Include glew to get all the required OpenGL Headers

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class Shader
{
public:
	// The program ID
	GLuint Program;

	// Constructor reads and builds the shader
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);

	virtual ~Shader();
	
	GLint getUniformPosition(const char* _varName);

	void setVec3(const char* _varName, glm::vec3 _value);
	void setFloat(const char* _varName, float _value);

	// Use the program
	void Use();
};
#endif

