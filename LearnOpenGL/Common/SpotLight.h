#ifndef SPOT_LIGHT_H
#define SPOT_LIGHT_H

#include "PointLight.h"

class SpotLight : public PointLight
{
public:

	SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 diffuse, glm::vec3 ambient, glm::vec3 specular, GLfloat constant, GLfloat linear, GLfloat quadratic, GLfloat cutOff, GLfloat outerCutOff)
		: PointLight(position, diffuse, ambient, specular, constant, linear, quadratic), CutOff(cutOff), OuterCutOff(outerCutOff), Direction(direction)
	{}

	~SpotLight() {}

	GLfloat CutOff;
	GLfloat OuterCutOff;
	glm::vec3 Direction;
};

#endif