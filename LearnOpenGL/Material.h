#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/vec3.hpp>
#include <GL/glew.h>

#define USING_DIFFUSE_MAP 1

class Material
{
public:

#if !USING_DIFFUSE_MAP
	Material(glm::vec3 diffuse, glm::vec3 ambient, glm::vec3 specular, GLfloat shininess) :
		Diffuse(diffuse), 
		Ambient(ambient), 
		Specular(specular), 
		Shininess(shininess)
	{}

	glm::vec3 Diffuse;
	glm::vec3 Ambient;
#else
	Material(GLuint diffuseMap, glm::vec3 specular, GLfloat shininess) :
		diffuseMapIndex(diffuseMap),
		Specular(specular),
		Shininess(shininess)
	{}

	GLuint diffuseMapIndex;
#endif

	Material() {}
	
	glm::vec3 Specular;
	GLfloat Shininess;
};

#endif