#ifndef DIR_LIGHT_H
#define DIR_LIGHT_H

#include <glm/vec3.hpp>
#include <GL/glew.h>
#include "Light.h"

class DirLight : public Light
{

public:
	DirLight() {}
	DirLight(glm::vec3 direction) : Direction(direction) {}
	DirLight(glm::vec3 direction, glm::vec3 diffuse, glm::vec3 ambient, glm::vec3 specular)
		: Direction(direction)
	{
		Diffuse = diffuse;
		Ambient = ambient;
		Specular = specular;
	}

	glm::vec3 Direction;
};
#endif
