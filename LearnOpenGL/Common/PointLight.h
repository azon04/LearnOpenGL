#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

#include "Light.h"

class PointLight : public Light
{
public:
	PointLight(glm::vec3 position, glm::vec3 diffuse, glm::vec3 ambient, glm::vec3 specular, GLfloat constant, GLfloat linear, GLfloat quadratic)
		: Light(position, diffuse, ambient, specular), Constant(constant), Linear(linear), Quadratic(quadratic)
	{}

	~PointLight();

	GLfloat Constant;
	GLfloat Linear;
	GLfloat Quadratic;
};

#endif
