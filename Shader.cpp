#include "Shader.hpp"

Shader::Shader() {
	shaderId = 0;
	uniformModel = 0;
	uniformProjection = 0;
	uniformView = 0;
	pointLightCount = 0;
	spotLightCount = 0;
}

Shader::~Shader() {
	clearShader();
}

void Shader::createFromFile(std::string vFile, std::string fFile) {
	std::ifstream fileStream(vFile, std::ios::in);

	if (!fileStream.is_open()) {
		std::cout << "File: " << vFile << " does not exist" << std::endl;
		throw;
	}

	// Read the vertex shader file
	std::string vFileContents = "";
	std::string line = "";
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		vFileContents.append(line + "\n");
	}
	fileStream.close();

	fileStream = std::ifstream(fFile, std::ios::in);

	if (!fileStream.is_open()) {
		throw ("File: " + fFile + " does not exist");
	}

	// Read the fragment shader file
	std::string fFileContents = "";
	line = "";
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		fFileContents.append(line + "\n");
	}
	fileStream.close();

	compileShader(vFileContents, fFileContents);
}

void Shader::createFromString(
	std::string vertexCode, 
	std::string fragmentCode
) {
	compileShader(vertexCode, fragmentCode);
}

GLuint Shader::getModelLocation() {
	return uniformModel;
}

GLuint Shader::getProjectionLocation() {
	return uniformProjection;
}

GLuint Shader::getViewLocation() {
	return uniformView;
}

GLuint Shader::getAmbientIntensityLocation() {
	return uniformDirectionalLight.uniformAmbientIntensity;
}

GLuint Shader::getColorLocation() {
	return uniformDirectionalLight.uniformColor;
}

GLuint Shader::getDiffuseIntensity() {
	return uniformDirectionalLight.uniformDiffuseIntensity;
}


GLuint Shader::getDirectionLocation() {
	return uniformDirectionalLight.uniformDirection;
}

GLuint Shader::getSpecularIntensityLocation() {
	return uniformSpecularIntensity;
}

GLuint Shader::getShininessLocation() {
	return uniformShininess;
}

GLuint Shader::getEyePositionLocation() {
	return uniformEyePosition;
}

// Not used
GLuint Shader::getScaleMatrixLocation() {
	return uniformScaleMatrix;
}

void Shader::setDirectionalLight(DirectionalLight *dLight) {
	dLight->useLight(
		uniformDirectionalLight.uniformAmbientIntensity,
		uniformDirectionalLight.uniformColor,
		uniformDirectionalLight.uniformDiffuseIntensity,
		uniformDirectionalLight.uniformDirection); 
}

void Shader::setPointLights(
	PointLight *pointLight,
	unsigned int lightCount
) {
	if (lightCount > MAX_POINT_LIGHTS) {
		lightCount = MAX_POINT_LIGHTS;
	} 

	glUniform1i(uniformPointLightCount, lightCount);

	for (int i = 0; i < MAX_POINT_LIGHTS; ++i) {
		pointLight[i].useLight(
			uniformPointLight[i].uniformAmbientIntensity,
			uniformPointLight[i].uniformColor,
			uniformPointLight[i].uniformDiffuseIntensity,
			uniformPointLight[i].uniformPosition,
			uniformPointLight[i].uniformConstant,
			uniformPointLight[i].uniformLinear,
			uniformPointLight[i].uniformQuadratic
		);
	}
}

void Shader::setSpotLights(
	SpotLight *spotLight, 
	unsigned int lightCount
) {
	if (lightCount > MAX_SPOT_LIGHTS) {
		lightCount = MAX_SPOT_LIGHTS;
	}

	glUniform1i(uniformSpotLightCount, lightCount);

	for (int i = 0; i < MAX_SPOT_LIGHTS; ++i) {
		spotLight[i].useLight(
			uniformSpotLight[i].uniformAmbientIntensity,
			uniformSpotLight[i].uniformColor,
			uniformSpotLight[i].uniformDiffuseIntensity,
			uniformSpotLight[i].uniformPosition,
			uniformSpotLight[i].uniformDirection,
			uniformSpotLight[i].uniformConstant,
			uniformSpotLight[i].uniformLinear,
			uniformSpotLight[i].uniformQuadratic,
			uniformSpotLight[i].uniformEdge
		);
	}
}


void Shader::useShader() {
	if (!shaderId) {
		throw "No shader present to use";
	}

	glUseProgram(shaderId);
}

void Shader::clearShader() {
	if (shaderId != 0) {
		glDeleteProgram(shaderId);
	}
	shaderId = 0;
	uniformModel = 0;
	uniformProjection = 0;
	uniformView = 0;
}

void Shader::compileShader(
	std::string vertexCode,
	std::string fragmentCode
) {
	// Creates the Program Object and returns a reference ID
	shaderId = glCreateProgram();

	// Make sure the shader was created correctly
	if (!shaderId) {
		std::cout << "Error creating shader" << std::endl;
		return;
	}

	// add the shaders we made to the program
	addShader(shaderId, vertexCode, GL_VERTEX_SHADER);
	addShader(shaderId, fragmentCode, GL_FRAGMENT_SHADER);

	// Get the error codes from the creation of the shaders
	GLint result = 0;
	GLchar errorLog[1024] = { 0 };

	// pass in our id generated from glCreateProgram() to the program linker
	// actually creates the executables on the graphics card
	glLinkProgram(shaderId);

	// This is actually for debugging
	// A lot of messages and codes are generated
	// 1. pass shader id,
	// 2. which bit of information we want to get back from it (GL_LINK_STATUS)
	// 3. where we want it to store the result of that
	glGetProgramiv(shaderId, GL_LINK_STATUS, &result);

	if (!result) {
		glGetProgramInfoLog(shaderId, sizeof(errorLog), NULL, errorLog);
		std::cout << "Error Linking shader program " << errorLog << std::endl;
		return;
	}

	// Checking for us if the shader we created is valid
	glValidateProgram(shaderId);
	glGetProgramiv(shaderId, GL_VALIDATE_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shaderId, sizeof(errorLog), NULL, errorLog);
		std::cout << "Error validating shader program " << errorLog << std::endl;
		return;
	}

	// 1. shader program itself
	// 2. search the program for the xMovement uniform variable
	// We want the id of the location of the uniform variable in the program
	uniformModel = glGetUniformLocation(shaderId, "model");
	uniformProjection = glGetUniformLocation(shaderId, "projection");
	uniformView = glGetUniformLocation(shaderId, "view");
	uniformScaleMatrix = glGetUniformLocation(shaderId, "scaleMatrix");

	// Configuration the uniforms for the Directional Light
	uniformDirectionalLight.uniformColor = glGetUniformLocation(shaderId, 
		"directionalLight.basicLight.color");

	uniformDirectionalLight.uniformAmbientIntensity = glGetUniformLocation(shaderId, 
		"directionalLight.basicLight.ambientIntensity");

	uniformDirectionalLight.uniformDirection = glGetUniformLocation(shaderId, 
		"directionalLight.direction");

	uniformDirectionalLight.uniformDiffuseIntensity = glGetUniformLocation(shaderId, 
		"directionalLight.basicLight.diffuseIntensity");

	//uniformColor = glGetUniformLocation(shader, "newColor");
	uniformSpecularIntensity = glGetUniformLocation(shaderId,
		"material.specularIntensity");
	uniformShininess = glGetUniformLocation(shaderId,
		"material.shininess");
	uniformEyePosition = glGetUniformLocation(shaderId,
		"eyePosition");

	uniformPointLightCount = glGetUniformLocation(shaderId, 
		"pointLightCount");

	uniformSpotLightCount = glGetUniformLocation(shaderId,
		"spotLightCount");

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "Error binding Uniform Values " << error << std::endl;
	}

	// For the point lights
	for (int i = 0; i < MAX_POINT_LIGHTS; ++i) {
		char locationBuffer[99] = { '\0' };
		//locationBuffer = std::string();
		//locationBuffer = std::format("pointLights[{}].color", i);
		//locationBuffer[99] = { '\0' };
		snprintf(locationBuffer, sizeof(locationBuffer),
			"pointLights[%d].basicLight.color", i);
		uniformPointLight[i].uniformColor = glGetUniformLocation(shaderId, locationBuffer);

		snprintf(locationBuffer, sizeof(locationBuffer),
			"pointLights[%d].basicLight.ambientIntensity", i);
		uniformPointLight[i].uniformAmbientIntensity = glGetUniformLocation(shaderId,locationBuffer);

		snprintf(locationBuffer, sizeof(locationBuffer),
			"pointLights[%d].basicLight.diffuseIntensity", i);
		uniformPointLight[i].uniformDiffuseIntensity = glGetUniformLocation(shaderId,locationBuffer);

		snprintf(locationBuffer, sizeof(locationBuffer),
			"pointLights[%d].position", i);
		uniformPointLight[i].uniformPosition = glGetUniformLocation(shaderId, locationBuffer);

		snprintf(locationBuffer, sizeof(locationBuffer),
			"pointLights[%d].constant", i);
		uniformPointLight[i].uniformConstant = glGetUniformLocation(shaderId, locationBuffer);

		snprintf(locationBuffer, sizeof(locationBuffer),
			"pointLights[%d].linear", i);
		uniformPointLight[i].uniformLinear = glGetUniformLocation(shaderId, locationBuffer);

		snprintf(locationBuffer, sizeof(locationBuffer),
			"pointLights[%d].quadratic", i);
		uniformPointLight[i].uniformQuadratic = glGetUniformLocation(shaderId, locationBuffer);
	}

	error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "Error binding uniform pointLight values " << error << std::endl;
	}

	// For the spot lights
	for (int i = 0; i < MAX_SPOT_LIGHTS; ++i) {
		char locationBuffer[99] = { '\0' };

		snprintf(locationBuffer, sizeof(locationBuffer),
			"spotLights[%d].basePointLight.basicLight.color", i);
		uniformSpotLight[i].uniformColor = glGetUniformLocation(shaderId, locationBuffer);

		snprintf(locationBuffer, sizeof(locationBuffer),
			"spotLights[%d].basePointLight.basicLight.ambientIntensity", i);
		uniformSpotLight[i].uniformAmbientIntensity = glGetUniformLocation(shaderId, locationBuffer);

		snprintf(locationBuffer, sizeof(locationBuffer),
			"spotLights[%d].basePointLight.basicLight.diffuseIntensity", i);
		uniformSpotLight[i].uniformDiffuseIntensity = glGetUniformLocation(shaderId, locationBuffer);

		snprintf(locationBuffer, sizeof(locationBuffer),
			"spotLights[%d].basePointLight.position", i);
		uniformSpotLight[i].uniformPosition = glGetUniformLocation(shaderId, locationBuffer);

		snprintf(locationBuffer, sizeof(locationBuffer),
			"spotLights[%d].basePointLight.constant", i);
		uniformSpotLight[i].uniformConstant = glGetUniformLocation(shaderId, locationBuffer);

		snprintf(locationBuffer, sizeof(locationBuffer),
			"spotLights[%d].basePointLight.linear", i);
		uniformSpotLight[i].uniformLinear = glGetUniformLocation(shaderId, locationBuffer);

		snprintf(locationBuffer, sizeof(locationBuffer),
			"spotLights[%d].basePointLight.quadratic", i);
		uniformSpotLight[i].uniformQuadratic = glGetUniformLocation(shaderId, locationBuffer);
	
		snprintf(locationBuffer, sizeof(locationBuffer),
			"spotLights[%d].direction", i);
		uniformSpotLight[i].uniformDirection = glGetUniformLocation(shaderId, locationBuffer);

		snprintf(locationBuffer, sizeof(locationBuffer),
			"spotLights[%d].edge", i);
		uniformSpotLight[i].uniformEdge = glGetUniformLocation(shaderId, locationBuffer);
	}

	error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "Error binding uniform pointLight values " << error << std::endl;
	}

}

void Shader::addShader(
	GLuint programId,
	std::string shaderCode,
	GLenum shaderType
) {
	// create the individual shader
	// create an empty shader for that type
	GLuint _shader = glCreateShader(shaderType);

	// pointer to the first value in the array
	const GLchar* shaderCodePointer[1];
	shaderCodePointer[0] = (GLchar*)shaderCode.c_str();

	// this is needed for the shader
	GLint shaderCodeStrLength[1];
	shaderCodeStrLength[0] = (GLint)shaderCode.length();

	// 1. the shader we are going to be modifying the code for
	// 2. we are only passing in one shader code
	// 3. the shader 'code' itself, 
	// 4. the character length of the program
	glShaderSource(_shader, 1, shaderCodePointer, shaderCodeStrLength);
	glCompileShader(_shader);

	// Before we attach it to the program itself, we need to do more error checking
	GLint result = 0;
	GLchar errorLog[1024] = { 0 };
	glGetShaderiv(_shader, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(shaderId, sizeof(errorLog), NULL, errorLog);
		std::cout << "Error compiling shader type: "
			<< shaderType << " with error: "
			<< errorLog << std::endl;
		return;
	}

	// Now that everything is compiled we can attach to the program
	glAttachShader(programId, _shader);
}

