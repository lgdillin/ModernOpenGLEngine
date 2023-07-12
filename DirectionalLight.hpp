#pragma once

#include "Light.hpp"


class DirectionalLight : public Light {
public:
	//DirectionalLight();
	DirectionalLight(
		glm::vec3 _direction = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 _color = glm::vec3(1.0f, 1.0f, 1.0f),
		GLfloat dIntensity = 0.2f,
		GLfloat aIntensity = 0.6f);

	~DirectionalLight();

	void useLight(GLuint ambientIntensityLocation, GLuint colorLocation,
		GLfloat diffuseIntensityLocation, GLfloat directionLocation);

private:
	glm::vec3 direction;
};