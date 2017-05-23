#ifndef LIGHT_H
#define LIGHT_H

#include <glm/vec3.hpp>
#include <GL/glew.h>

class Light {
public:
	Light(glm::vec3 position, glm::vec3 diffuse, glm::vec3 ambient, glm::vec3 specular)
		: Position(position), Diffuse(diffuse), Ambient(ambient), Specular(specular)
	{}

	Light(glm::vec3 position) :
		Position(position) {}

	Light() {}

	glm::vec3 Position;
	glm::vec3 Diffuse;
	glm::vec3 Ambient;
	glm::vec3 Specular;
};
#endif // 
