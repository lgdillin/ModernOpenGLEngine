#include "DirectionalLight.hpp"

//DirectionalLight::DirectionalLight() {}

DirectionalLight::DirectionalLight(
	glm::vec3 _direction,
	glm::vec3 _color,
	GLfloat aIntensity,
	GLfloat dIntensity
) : Light(_color, aIntensity, dIntensity) {
	direction = glm::vec3(_direction.x, _direction.y, _direction.z);
}

DirectionalLight::~DirectionalLight() {

}

void DirectionalLight::useLight(
	GLuint ambientIntensityLocation, 
	GLuint colorLocation,
	GLfloat diffuseIntensityLocation, 
	GLfloat directionLocation
) {

	glUniform3f(colorLocation, color.r, color.g, color.b);
	glUniform1f(ambientIntensityLocation, ambientIntensity);

	glUniform3f(directionLocation, direction.x, direction.y, direction.z);
	glUniform1f(diffuseIntensityLocation, diffuseIntensity);
}