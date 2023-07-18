#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "ShadowMap.hpp"

class Light {
public:
	Light(
		glm::vec3 _color = glm::vec3(1.0f, 1.0f, 1.0f), 
		GLfloat dIntensity = 0.2f,
		GLfloat aIntensity = 1.0f,
		GLuint _shadowWidth = 1024,
		GLuint _shadowHeight = 1024);
	
	
	~Light();

	void initShadowMap();
	ShadowMap *getShadowMap() { return m_shadowMap; }

	//void useLight(
	//	GLuint ambientIntensityLocation,
	//	GLuint colorLocation,
	//	GLfloat diffuseIntensityLocation,
	//	GLfloat directionLocation,
	//	GLfloat constantLocation = 0.0f,
	//	GLfloat linearLocation = 0.0f,
	//	GLfloat quadraticLocation = 0.0f
	//);

// protected means that these values can be accessed by subclasses,
// but not anything else
protected:
	glm::vec3 color;
	GLfloat ambientIntensity, diffuseIntensity;

	ShadowMap *m_shadowMap;
	GLuint m_shadowWidth, m_shadowHeight;

	glm::mat4 m_lightProjectionMat;
};