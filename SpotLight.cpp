#include "SpotLight.hpp"

SpotLight::SpotLight(
	glm::vec3 _position, 
	glm::vec3 _direction,
	glm::vec3 _color, 
	GLfloat dIntensity, 
	GLfloat aIntensity, 
	GLfloat _constant, 
	GLfloat _linear, 
	GLfloat _quadratic,
	GLfloat _edge
) : PointLight(
	_position, 
	_color, 
	aIntensity, 
	dIntensity,
	_constant,
	_linear,
	_quadratic
){
	// directional vector
	direction = _direction;
	edge = _edge;
	processedEdge = cosf(glm::radians(edge));

}

SpotLight::~SpotLight() {
}

void SpotLight::useLight(
	GLuint ambientIntensityLocation, 
	GLuint colorLocation, 
	GLfloat diffuseIntensityLocation, 
	GLfloat positionLocation, 
	GLfloat directionLocation,
	GLfloat constantLocation, 
	GLfloat linearLocation, 
	GLfloat quadraticLocation,
	GLuint edgeLocation
) {
	glUniform3f(colorLocation, color.r, color.g, color.b);
	glUniform1f(ambientIntensityLocation, ambientIntensity);

	glUniform3f(positionLocation, position.x, position.y, position.z);
	glUniform1f(diffuseIntensityLocation, diffuseIntensity);

	glUniform1f(constantLocation, constant);
	glUniform1f(linearLocation, linear);
	glUniform1f(quadraticLocation, quadratic);

	glUniform3f(directionLocation, direction.x, direction.y, direction.z);
	glUniform1f(edgeLocation, processedEdge);
}

void SpotLight::setEdge(GLfloat _edge) {
	edge = _edge;
	processedEdge = cosf(glm::radians(edge));
}

void SpotLight::setColor(glm::vec3 _color) {
	color = _color;
}

void SpotLight::setDirection(glm::vec3 _direction) {
	direction = _direction;
}

void SpotLight::setConstantFactor(GLfloat _constant) {
	constant = _constant;
}

void SpotLight::setLinearFactor(GLfloat _linear) {
	linear = _linear;
}

void SpotLight::setQuadraticFactor(GLfloat _quadratic) {
	quadratic = _quadratic;
}

void SpotLight::setFlash(glm::vec3 _position, glm::vec3 _direction) {
	position = _position;
	direction = _direction;
}


