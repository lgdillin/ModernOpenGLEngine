#pragma once

#include "Light.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "OmniShadowMap.hpp"

class PointLight : public Light {
public:
	PointLight(
		glm::vec3 _position = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 _color = glm::vec3(1.0f, 1.0f, 1.0f),
		GLfloat dIntensity = 0.3f,
		GLfloat aIntensity = 0.6f,
		GLfloat _constant = 0.5f,
		GLfloat _linear = 0.2f,
		GLfloat _quadratic = 0.1f,
		GLfloat _shadowWidth = 1024.0f,
		GLfloat _shadowHeight = 1024.0f,
		GLfloat _nearPlane = 0.1f,
		GLfloat _farPlane = 100.0f
	);

	~PointLight();

	void initOmniShadowMap();

	void setIntensity(GLfloat dIntensity = 0.3f, GLfloat aIntensity = 0.6f);
	void setFalloff(GLfloat _constant = 0.5f, GLfloat _linear = 0.2f, GLfloat _quadratic = 0.1f);

	void useLight(
		GLuint ambientIntensityLocation,
		GLuint colorLocation,
		GLfloat diffuseIntensityLocation,
		GLfloat positionLocation,
		GLfloat constantLocation,
		GLfloat linearLocation,
		GLfloat quadraticLocation);

	std::vector<glm::mat4> calculateLightTransform();
	
	
	GLfloat getFarPlane();
	glm::vec3 getPosition() { return position; }
	void setPosition(glm::vec3 _position);

protected:
	glm::vec3 position;

	GLfloat constant, linear, quadratic, nearPlane, farPlane;
};