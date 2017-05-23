#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/vec3.hpp>
#include <GL/glew.h>

class Material
{
public:
	Material(glm::vec3 diffuse, glm::vec3 ambient, glm::vec3 specular, GLfloat shininess) :
		Diffuse(diffuse), Ambient(ambient), Specular(specular), Shininess(shininess)
	{}

	Material() {}
	
	glm::vec3 Diffuse;
	glm::vec3 Ambient;
	glm::vec3 Specular;
	GLfloat Shininess;
};

#endif