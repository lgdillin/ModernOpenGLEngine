#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class Light {
public:
	Light();
	Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity);
	~Light();

	void useLight(GLuint ambientIntensityLocation,
		GLuint ambientColorLocation);
private:
	glm::vec3 color;
	GLfloat ambientIntensity;
};