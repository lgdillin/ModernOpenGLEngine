#pragma once

#include "PointLight.hpp"

class SpotLight : public PointLight {
public:
	//SpotLight();

	// TODO: change all references of 'edge' to 'angle'

	SpotLight(
		glm::vec3 _position = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 _direction = glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3 _color = glm::vec3(1.0f, 1.0f, 1.0f),
		GLfloat dIntensity = 0.1f,
		GLfloat aIntensity = 0.5f,
		GLfloat _constant = 0.01f,
		GLfloat _linear = 0.05f,
		GLfloat _quadratic = 0.005f,
		GLfloat _edge = 1.3f
	);

	~SpotLight();

	void useLight(
		GLuint ambientIntensityLocation,
		GLuint colorLocation,
		GLfloat diffuseIntensityLocation,
		GLfloat positionLocation,
		GLfloat directionLocation,
		GLfloat constantLocation,
		GLfloat linearLocation,
		GLfloat quadraticLocation,
		GLuint edgeLocation);

	void setEdge(GLfloat _edge);
	void setColor(glm::vec3 _color);
	void setDirection(glm::vec3 _direction);
	void setConstantFactor(GLfloat _constant);
	void setLinearFactor(GLfloat _linear);
	void setQuadraticFactor(GLfloat _quadratic);

	void setFlash(glm::vec3 _position, glm::vec3 _direction);

private:
	glm::vec3 direction;


	// stores the angle of where the edge of the spotlight is
	GLfloat edge;
	// store the processed edge post-shader computation
	GLfloat processedEdge;
};