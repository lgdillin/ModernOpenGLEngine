#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class Light {
public:
	Light(
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f), 
		GLfloat dIntensity = 0.2f,
		GLfloat aIntensity = 1.0f);
	
	
	~Light();

	virtual void useLight(
		GLuint ambientIntensityLocation,
		GLuint colorLocation,
		GLfloat diffuseIntensityLocation,
		GLfloat directionLocation) = 0;

// protected means that these values can be accessed by subclasses,
// but not anything else
protected:
	glm::vec3 color;
	GLfloat ambientIntensity, diffuseIntensity;
};