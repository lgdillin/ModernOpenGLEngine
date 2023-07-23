#include "PointLight.hpp"

PointLight::PointLight(
	glm::vec3 _position,
	glm::vec3 _color,
	GLfloat dIntensity,
	GLfloat aIntensity,
	GLfloat _constant,
	GLfloat _linear,
	GLfloat _quadratic,
	GLfloat _shadowWidth,
	GLfloat _shadowHeight,
	GLfloat _nearPlane,
	GLfloat _farPlane
) : Light(
	_color, 
	aIntensity, 
	dIntensity,
	_shadowWidth,
	_shadowHeight
) {
	position = glm::vec3(_position);
	constant = _constant;
	linear = _linear;
	quadratic = _quadratic;

	nearPlane = _nearPlane;
	farPlane = _farPlane;

	float aspectRatio = (float)(m_shadowWidth / m_shadowHeight);
	m_lightProjectionMat = glm::perspective(
		glm::radians(90.0f),
		aspectRatio,
		nearPlane, 
		farPlane
	);

	
}

PointLight::~PointLight() {

}

void PointLight::initOmniShadowMap() {
	m_shadowMap = new OmniShadowMap();
	m_shadowMap->init(m_shadowWidth, m_shadowHeight);
}

void PointLight::setIntensity(GLfloat _dIntensity, GLfloat _aIntensity) {
	diffuseIntensity = _dIntensity;
	ambientIntensity = _aIntensity;
}

void PointLight::setFalloff(GLfloat _constant, GLfloat _linear, GLfloat _quadratic) {
	constant = _constant;
	linear = _linear;
	quadratic = _quadratic;
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

std::vector<glm::mat4> PointLight::calculateLightTransform() {
	std::vector<glm::mat4> lightMatrices;

	// We may need to invert the 'up' vector (0.0, 1.0, 0.0) due to 
	// how lights are processed

	// Positive X
	lightMatrices.push_back(m_lightProjectionMat * glm::lookAt(
		position,
		position + glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f)));

	// Negative X
	lightMatrices.push_back(m_lightProjectionMat * glm::lookAt(
		position,
		position + glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f)));

	// Positive Y
	lightMatrices.push_back(m_lightProjectionMat * glm::lookAt(
		position,
		position + glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, -1.0f)));

	// Negative Y
	lightMatrices.push_back(m_lightProjectionMat * glm::lookAt(
		position,
		position + glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, -1.0f)));

	// Positive Z
	lightMatrices.push_back(m_lightProjectionMat * glm::lookAt(
		position,
		position + glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, -1.0f, 0.0f)));

	// Negative Z
	lightMatrices.push_back(m_lightProjectionMat * glm::lookAt(
		position,
		position + glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, -1.0f, 0.0f)));

	return lightMatrices;
}

GLfloat PointLight::getFarPlane() {
	return farPlane;
}

void PointLight::setPosition(glm::vec3 _position) {
	position = _position;
}
