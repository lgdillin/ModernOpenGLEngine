#pragma once

#include <iostream>
#include <format>
#include <string>
#include <fstream>

#include <GL/glew.h>

#include "PointLight.hpp"
#include "DirectionalLight.hpp"

#define MAX_POINT_LIGHTS 3

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

	void setDirectionalLight(DirectionalLight *dLight);
	void setPointLights(PointLight *pointLight, unsigned int lightCount);

	void useShader();
	void clearShader();

private:

	struct {
		GLuint uniformColor;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;

		GLuint uniformDirection;
	} uniformDirectionalLight;

	struct {
		GLuint uniformColor;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;

		GLuint uniformPosition;
		GLuint uniformConstant;
		GLuint uniformLinear;
		GLuint uniformQuadratic;
	} uniformPointLight[MAX_POINT_LIGHTS];

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

	int pointLightCount;

	GLuint 
		shaderId, 
		uniformProjection, 
		uniformModel,
		uniformView,
		uniformScaleMatrix,
		//uniformAmbientIntensity,
		//uniformColor,
		//uniformDiffuseIntensity,
		//uniformDirection,
		uniformEyePosition,
		uniformSpecularIntensity,
		uniformShininess,
		uniformPointLightCount;

};