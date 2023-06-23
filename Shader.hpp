#pragma once

#include <iostream>
#include <string>
#include <fstream>

#include <GL/glew.h>

class Shader {
public:
	Shader();
	~Shader();

	void createFromFile(std::string vFile, std::string fFile);
	void createFromString(std::string vertexCode, std::string fragmentCode);

	GLuint getModelLocation();
	GLuint getProjectionLocation();
	GLuint getViewLocation();
	GLuint getAmbientIntensityLocation();
	GLuint getColorLocation();
	GLuint getScaleMatrixLocation();
	GLuint getDiffuseIntensity();
	GLuint getDirectionLocation();
	GLuint getSpecularIntensityLocation();
	GLuint getShininessLocation();
	GLuint getEyePositionLocation();

	GLuint getShaderId() { return shaderId; }

	void useShader();
	void clearShader();

private:
	// compiles a shader from a string literal or from a read file
	void compileShader(
		std::string vertexCode, 
		std::string fragmentCode
	);

	void addShader(
		GLuint programId, 
		std::string shaderCode, 
		GLenum shaderType
	);

	GLuint 
		shaderId, 
		uniformProjection, 
		uniformModel,
		uniformView,
		uniformScaleMatrix,
		uniformAmbientIntensity,
		uniformColor,
		uniformDiffuseIntensity,
		uniformDirection,
		uniformEyePosition,
		uniformSpecularIntensity,
		uniformShininess;

};