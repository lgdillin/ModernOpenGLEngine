#pragma once

#include <iostream>
#include <format>
#include <string>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

#include "PointLight.hpp"
#include "DirectionalLight.hpp"
#include "SpotLight.hpp"

#define MAX_POINT_LIGHTS 3
#define MAX_SPOT_LIGHTS 4

class Shader {
public:
	Shader();
	~Shader();

	void createFromFile(std::string vFile, std::string fFile);
	void createFromFile(std::string vFile, std::string gFile, std::string fFile);

	void createFromString(std::string vertexCode, std::string fragmentCode);

	void setVec3(std::string _glslCode, glm::vec3 _vec3);
	void setFloat(std::string _glslCode, float _float);

	void validate();

	void setDirectionalLight(DirectionalLight *dLight);
	void setPointLights(PointLight *pointLight, unsigned int lightCount,
		unsigned int textureUnit, unsigned int offset);
	
	void setSpotLights(SpotLight *spotLight, unsigned int lightCount,
		unsigned int textureUnit, unsigned int offset);


	void setTexture(GLuint textureUnit);
	void setDirectionalShadowMap(GLuint textureUnit);
	void setDirectionalLightTransform(glm::mat4 transform);
	void setLightMatrices(std::vector<glm::mat4> lightMatrices);

	void useShader();
	void clearShader();

	GLuint getShaderId() { return shaderId; }
	GLuint getModelLocation() { return uniformModel; }
	GLuint getProjectionLocation() { return uniformProjection; }
	GLuint getViewLocation() { return uniformView; }
	GLuint getAmbientIntensityLocation() { return uniformDirectionalLight.uniformAmbientIntensity; }
	GLuint getColorLocation() { return uniformDirectionalLight.uniformColor; }
	GLuint getScaleMatrixLocation() { return uniformScaleMatrix; }
	GLuint getDiffuseIntensity() { return uniformDirectionalLight.uniformDiffuseIntensity; }
	GLuint getDirectionLocation() { return uniformDirectionalLight.uniformDirection; }
	GLuint getSpecularIntensityLocation() { return uniformSpecularIntensity; }
	GLuint getShininessLocation() { return uniformShininess; }
	GLuint getEyePositionLocation() { return uniformEyePosition; }
	GLuint getOmniLightPositionLocation() { return uniformOmniLightPosition; }
	GLuint getFarPlaneLocation() { return uniformFarPlane; }

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

	struct {
		GLuint uniformColor;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;

		GLuint uniformPosition;
		GLuint uniformDirection;
		GLuint uniformConstant;
		GLuint uniformLinear;
		GLuint uniformQuadratic;
		GLuint uniformEdge;
	} uniformSpotLight[MAX_SPOT_LIGHTS];

	struct {
		GLuint shadowMap;
		GLuint farPlane;
	} uniformOmniShadowMap[MAX_POINT_LIGHTS + MAX_SPOT_LIGHTS];

	// compiles a shader from a string literal or from a read file
	void compileShader(
		std::string vertexCode, 
		std::string fragmentCode
	);

	void compileShader(
		std::string vertexCode,
		std::string geometryCode,
		std::string fragmentCode
	);

	void compileLights();

	void compileProgram();

	void addShader(
		GLuint programId, 
		std::string shaderCode, 
		GLenum shaderType
	);

	int pointLightCount, spotLightCount;

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
		uniformPointLightCount,
		uniformSpotLightCount,
		uniformDirectionalLightTransform,
		uniformDirectionalShadowMap,
		uniformTexture,
		uniformOmniLightPosition,
		uniformFarPlane,
		uniformLightMatrices[6];

};