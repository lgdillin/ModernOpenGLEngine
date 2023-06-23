#include "Material.hpp"

Material::Material() {
	specularIntensity = 0.0f;
	shininess = 0.0f;
}

Material::Material(GLfloat sIntensity, GLfloat shine) {
	specularIntensity = sIntensity;
	shininess = shine;
}

Material::~Material() {

}


void Material::useMaterial(
	GLuint specularIntensityLocation, GLuint shininessLocation) {

	glUniform1f(specularIntensityLocation, specularIntensity);
	glUniform1f(shininessLocation, shininess);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) std::cout << error << std::endl;
	
}