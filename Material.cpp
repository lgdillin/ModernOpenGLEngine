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
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "Error in useMaterial() " << error << std::endl;
	}
	
	glUniform1f(shininessLocation, shininess);
	error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "Error in useMaterial() " << error << std::endl;
	}
	
}