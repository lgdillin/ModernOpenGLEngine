#pragma once

#include "Light.hpp"

class PointLight : public Light {
public:
	PointLight(
		glm::vec3 _position = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 _color = glm::vec3(1.0f, 1.0f, 1.0f),
		GLfloat dIntensity = 0.3f,
		GLfloat aIntensity = 2.0f,
		GLfloat _constant = 0.5f,
		GLfloat _linear = 0.2f,
		GLfloat _quadratic = 0.1f
	);

	~PointLight();

	void useLight(
		GLuint ambientIntensityLocation,
		GLuint colorLocation,
		GLfloat diffuseIntensityLocation,
		GLfloat positionLocation,
		GLfloat constantLocation,
		GLfloat linearLocation,
		GLfloat quadraticLocation);

private:
	glm::vec3 position;

	GLfloat constant, linear, quadratic;
};