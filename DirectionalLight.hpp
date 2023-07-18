#pragma once

#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Light.hpp"


class DirectionalLight : public Light {
public:
	//DirectionalLight();
	DirectionalLight(
		glm::vec3 _direction = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 _color = glm::vec3(1.0f, 0.9f, 0.9f),
		GLfloat dIntensity = 0.2f,
		GLfloat aIntensity = 0.6f,
		GLfloat _shadowWidth = 1024.0f,
		GLfloat _shadowHeight = 1024.0f);

	~DirectionalLight();

	void setDirection(glm::vec3 _dir);
	glm::vec3 getDirection() { return direction; }

	void useLight(GLuint ambientIntensityLocation, GLuint colorLocation,
		GLfloat diffuseIntensityLocation, GLfloat directionLocation);

	glm::mat4 calculateLightTransform();

private:
	glm::vec3 direction;
	glm::mat4 dLightTransformMatrix;
};