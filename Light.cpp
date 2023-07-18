#include "Light.hpp"

Light::Light(
	glm::vec3 _color,
	GLfloat aIntensity, 
	GLfloat dIntensity,
	GLuint _shadowWidth,
	GLuint _shadowHeight
) {
	color = glm::vec3(_color.r, _color.g, _color.b);
	ambientIntensity = aIntensity;

	diffuseIntensity = dIntensity;

	m_shadowWidth = _shadowWidth;
	m_shadowHeight = _shadowHeight;

	//m_shadowMap = new ShadowMap();
	//m_shadowMap->init(_shadowWidth, _shadowHeight);
	
}

Light::~Light() {

}

void Light::initShadowMap() {
	m_shadowMap = new ShadowMap();
	m_shadowMap->init(m_shadowWidth, m_shadowHeight);
}

//void Light::useLight(GLuint ambientIntensityLocation, GLuint colorLocation,
//	GLfloat diffuseIntensityLocation, GLfloat directionLocation
//) {
//
//	glUniform3f(colorLocation, color.x, color.y, color.z);
//	glUniform1f(ambientIntensityLocation, ambientIntensity);
//	
//	glUniform3f(directionLocation, direction.x, direction.y, direction.z);
//	glUniform1f(diffuseIntensityLocation, diffuseIntensity);
//}