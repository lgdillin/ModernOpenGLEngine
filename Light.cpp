#include "Light.hpp"

Light::Light(
	glm::vec3 _color,
	GLfloat aIntensity, 
	GLfloat dIntensity
) {
	color = glm::vec3(_color.r, _color.g, _color.b);
	ambientIntensity = aIntensity;

	diffuseIntensity = dIntensity;
}

Light::~Light() {

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