#pragma once

#include <iostream>
#include <GL/glew.h>

class Material {
public:
	Material();
	Material(GLfloat sIntensity, GLfloat shine);
	~Material();

	void useMaterial(
		GLuint specularIntensityLocation, GLuint shininessLocation);

private:
	GLfloat specularIntensity, shininess;
};