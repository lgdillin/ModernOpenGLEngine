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
		uniformModel;

};