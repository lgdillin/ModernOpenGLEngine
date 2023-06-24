#include "DirectionalLight.hpp"

#include <iostream>
#include <GL/glew.h>

//DirectionalLight::DirectionalLight() {}

DirectionalLight::DirectionalLight(
	glm::vec3 _direction,
	glm::vec3 _color,
	GLfloat aIntensity,
	GLfloat dIntensity
) : Light(_color, aIntensity, dIntensity) {
	direction = glm::vec3(_direction);
}

DirectionalLight::~DirectionalLight() {

}

void DirectionalLight::useLight(
	GLuint ambientIntensityLocation, 
	GLuint colorLocation,
	GLfloat diffuseIntensityLocation, 
	GLfloat directionLocation
) {
	//GLenum error = glGetError(); std::cout << error;
	glUniform3f(colorLocation, color.r, color.g, color.b);
	//error = glGetError(); std::cout << error;
	glUniform1f(ambientIntensityLocation, ambientIntensity);
	//error = glGetError(); std::cout << error;

	glUniform3f(directionLocation, direction.x, direction.y, direction.z);
	//error = glGetError(); std::cout << error;
	glUniform1f(diffuseIntensityLocation, diffuseIntensity);
	//error = glGetError(); std::cout << error;
	//std::cout <<std::endl;
}