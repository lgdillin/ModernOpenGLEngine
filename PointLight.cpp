#include "PointLight.hpp"

PointLight::PointLight(
	glm::vec3 _position,
	glm::vec3 _color,
	GLfloat dIntensity,
	GLfloat aIntensity,
	GLfloat _constant,
	GLfloat _linear,
	GLfloat _quadratic
) : Light(_color, aIntensity, dIntensity) {
	position = glm::vec3(_position);
	constant = _constant;
	linear = _linear;
	quadratic = _quadratic;
}

PointLight::~PointLight() {

}

void PointLight::useLight(
	GLuint ambientIntensityLocation,
	GLuint colorLocation,
	GLfloat diffuseIntensityLocation,
	GLfloat positionLocation,
	GLfloat constantLocation,
	GLfloat linearLocation,
	GLfloat quadraticLocation
) {
	glUniform3f(colorLocation, color.r, color.g, color.b);
	glUniform1f(ambientIntensityLocation, ambientIntensity);

	glUniform3f(positionLocation, position.x, position.y, position.z);
	glUniform1f(diffuseIntensityLocation, diffuseIntensity);

	glUniform1f(constantLocation, constant);
	glUniform1f(linearLocation, linear);
	glUniform1f(quadraticLocation, quadratic);
}