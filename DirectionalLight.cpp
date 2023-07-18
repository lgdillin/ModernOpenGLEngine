#include "DirectionalLight.hpp"

//DirectionalLight::DirectionalLight() {}

DirectionalLight::DirectionalLight(
	glm::vec3 _direction,
	glm::vec3 _color,
	GLfloat aIntensity,
	GLfloat dIntensity,
	GLfloat _shadowWidth,
	GLfloat _shadowHeight
) : Light(
	_color, 
	aIntensity, 
	dIntensity,
	_shadowWidth,
	_shadowHeight
) {

	dLightTransformMatrix = glm::mat4(1.0f);
	direction = glm::vec3(_direction);
	m_lightProjectionMat = glm::ortho(
		-50.0f, // left
		50.0f, // right
		-50.0f, // bottom
		50.0f, // top
		0.01f, // near
		150.0f // far
	);
}

DirectionalLight::~DirectionalLight() {

}

void DirectionalLight::setDirection(glm::vec3 _dir) {
	direction = _dir;
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

glm::mat4 DirectionalLight::calculateLightTransform() {
	// by using -direction, we are going in the opposite direction
	// of the point being lit, to the thing that is lighting it
	// we can fake an eye/position by using -direction

	// we're now going from the opposite direction to the origin
	// which is basically the origin
	dLightTransformMatrix = m_lightProjectionMat * glm::lookAt(direction,
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	return dLightTransformMatrix;
}
