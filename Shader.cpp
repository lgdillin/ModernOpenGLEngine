#include "Shader.hpp"

void errorReporter(std::string errorLocationPrefix) {
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << errorLocationPrefix << " " 
			<< error << std::endl;
	}
}

Shader::Shader() {
	m_shaderId = 0;
	uniformModel = 0;
	uniformProjection = 0;
	uniformView = 0;
	pointLightCount = 0;
	spotLightCount = 0;
	uniformDirectionalLightTransform = 99;
}

Shader::~Shader() {
	clearShader();
}

void Shader::createFromFile(
	std::string vFile, 
	std::string fFile,
	SHADING_MODE _mode
) {
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

	if (_mode == FORWARD_SHADING) {
		compileShader(vFileContents, fFileContents);
	} else if (_mode == DEFERRED_SHADING) {
		compileDeferredShader(vFileContents, fFileContents);
	}
}

void Shader::createFromFile(
	std::string vFile, 
	std::string gFile, 
	std::string fFile,
	SHADING_MODE _mode
) {
	// Read the vertex shader file
	std::ifstream fileStream(vFile, std::ios::in);
	if (!fileStream.is_open()) {
		std::cout << "File: " << vFile << " does not exist" << std::endl;
		throw;
	}

	std::string vFileContents = "";
	std::string line = "";
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		vFileContents.append(line + "\n");
	}
	fileStream.close();

	// Read the Geometry shader file
	fileStream = std::ifstream(gFile, std::ios::in);
	if (!fileStream.is_open()) {
		std::cout << "File: " << gFile << " does not exist" << std::endl;
		throw;
	}

	std::string gFileContents = "";
	line = "";
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		gFileContents.append(line + "\n");
	}
	fileStream.close();

	// Read Fragment Shader
	fileStream = std::ifstream(fFile, std::ios::in);
	if (!fileStream.is_open()) {
		throw ("File: " + fFile + " does not exist");
	}

	std::string fFileContents = "";
	line = "";
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		fFileContents.append(line + "\n");
	}
	fileStream.close();

	if (_mode == FORWARD_SHADING) {
		compileShader(vFileContents, gFileContents, fFileContents);
	} else if (_mode == DEFERRED_SHADING) {
		//compileDeferredShader(vFileContents, fFileContents);
		std::cout << "Make geometry mode" << std::endl;
	}
}

void Shader::createFromString(
	std::string vertexCode, 
	std::string fragmentCode
) {
	compileShader(vertexCode, fragmentCode);
}

void Shader::setMat4(std::string _glslCode, glm::mat4 _mat4) {
	GLuint uniformLocation = glGetUniformLocation(m_shaderId,
		_glslCode.c_str());
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(_mat4));
}

void Shader::setVec3(std::string _glslCode, glm::vec3 _vec3) {
	GLuint uniformLocation = glGetUniformLocation(m_shaderId,
		_glslCode.c_str());
	glUniform3f(uniformLocation, _vec3.x, _vec3.y, _vec3.z);
}

void Shader::setFloat(std::string _glslCode, float _float) {
	GLuint uniformLocation = glGetUniformLocation(m_shaderId,
		_glslCode.c_str());
	glUniform1f(uniformLocation, _float);
}

void Shader::setTexture(std::string _glslCode, GLenum _textureUnit) {
	GLuint uniformLocation = glGetUniformLocation(m_shaderId,
		_glslCode.c_str());
	glUniform1i(uniformLocation, _textureUnit);
}

void Shader::validate() {
	// Get the error codes from the creation of the shaders
	GLint result = 0;
	GLchar errorLog[1024] = { 0 };

	// Checking for us if the shader we created is valid
	glValidateProgram(m_shaderId);
	glGetProgramiv(m_shaderId, GL_VALIDATE_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(m_shaderId, sizeof(errorLog), NULL, errorLog);
		std::cout << "Shader::validate() error: " << errorLog << std::endl;
		return;
	}
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
	unsigned int lightCount,
	unsigned int textureUnit,
	unsigned int offset
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

		pointLight[i].getShadowMap()->read(GL_TEXTURE0 + textureUnit + i);
		glUniform1i(uniformOmniShadowMap[i + offset].shadowMap, 
			textureUnit + i);

		glUniform1f(uniformOmniShadowMap[i + offset].farPlane,
			pointLight[i].getFarPlane());
	}

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "Shader::setPointLights() error "
			<< error << std::endl;
	}
}

void Shader::setSpotLights(
	SpotLight *spotLight, 
	unsigned int lightCount,
	unsigned int textureUnit,
	unsigned int offset
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

		spotLight[i].getShadowMap()->read(GL_TEXTURE0 + textureUnit + i);
		glUniform1i(uniformOmniShadowMap[i + offset].shadowMap,
			textureUnit + i);

		glUniform1f(uniformOmniShadowMap[i + offset].farPlane,
			spotLight[i].getFarPlane());
	}

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "Shader::setSpotLights() error "
			<< error << std::endl;
	}
}

void Shader::setTexture(GLuint textureUnit) {
	glUniform1i(uniformTexture, textureUnit);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "setTexture(GLuint textureUnit) error in Shader: " 
			<< error << std::endl;
	}
}

void Shader::setDirectionalShadowMap(GLuint textureUnit) {
	glUniform1i(uniformDirectionalShadowMap, textureUnit);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "Shader::setDirectionalShadowMap(GLuint textureUnit) error: " 
			<< error << std::endl;
	}
}

void Shader::setDirectionalLightTransform(glm::mat4 transform) {
	glUniformMatrix4fv(uniformDirectionalLightTransform, 1, GL_FALSE,
		glm::value_ptr(transform));

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "Shader::setDirectionalLightTransform() error: " 
			<< error << std::endl;
	}
}

void Shader::setLightMatrices(std::vector<glm::mat4> lightMatrices) {
	for (int i = 0; i < 6; ++i) {
		glUniformMatrix4fv(uniformLightMatrices[i], 1, GL_FALSE,
			glm::value_ptr(lightMatrices[i]));
	}
}

void Shader::setInt(std::string _name, int _value) {
	glUniform1i(glGetUniformLocation(m_shaderId, _name.c_str()), _value);
}


void Shader::useShader() {
	if (!m_shaderId) {
		throw "Shader::useShader(): No shader present to use";
	}

	glUseProgram(m_shaderId);
}

void Shader::clearShader() {
	if (m_shaderId != 0) {
		glDeleteProgram(m_shaderId);
	}
	m_shaderId = 0;
	uniformModel = 0;
	uniformProjection = 0;
	uniformView = 0;
}

void Shader::compileShader(
	std::string vertexCode,
	std::string fragmentCode
) {
	// Creates the Program Object and returns a reference ID
	m_shaderId = glCreateProgram();

	// Make sure the shader was created correctly
	if (!m_shaderId) {
		std::cout << "Error creating shader" << std::endl;
		return;
	}

	// add the shaders we made to the program
	addShader(m_shaderId, vertexCode, GL_VERTEX_SHADER);
	addShader(m_shaderId, fragmentCode, GL_FRAGMENT_SHADER);

	compileProgram();
}

void Shader::compileShader(std::string vertexCode, std::string geometryCode, std::string fragmentCode) {
	// Creates the Program Object and returns a reference ID
	m_shaderId = glCreateProgram();

	// Make sure the shader was created correctly
	if (!m_shaderId) {
		std::cout << "Error creating shader" << std::endl;
		return;
	}

	// add the shaders we made to the program
	addShader(m_shaderId, vertexCode, GL_VERTEX_SHADER);
	addShader(m_shaderId, geometryCode, GL_GEOMETRY_SHADER);
	addShader(m_shaderId, fragmentCode, GL_FRAGMENT_SHADER);

	compileProgram();
}

void Shader::compileDeferredShader(std::string vertexCode, std::string fragmentCode) {
	m_shaderId = glCreateProgram();

	if (!m_shaderId) {
		std::cout << "Shader::compileDeferredShader() Error executing glCreateProgram()"
			<< std::endl;
		return;
	}

	addShader(m_shaderId, vertexCode, GL_VERTEX_SHADER);
	addShader(m_shaderId, fragmentCode, GL_FRAGMENT_SHADER);

	compileDeferredProgram(); // Should be temp for now
}

void Shader::compileLights() {
	for (int i = 0; i < MAX_POINT_LIGHTS; ++i) {

	}
}

void Shader::compileProgram() {
	// Get the error codes from the creation of the shaders
	GLint result = 0;
	GLchar errorLog[1024] = { 0 };

	// pass in our id generated from glCreateProgram() to the program linker
	// actually creates the executables on the graphics card
	glLinkProgram(m_shaderId);

	// This is actually for debugging
	// A lot of messages and codes are generated
	// 1. pass shader id,
	// 2. which bit of information we want to get back from it (GL_LINK_STATUS)
	// 3. where we want it to store the result of that
	glGetProgramiv(m_shaderId, GL_LINK_STATUS, &result);

	if (!result) {
		glGetProgramInfoLog(m_shaderId, sizeof(errorLog), NULL, errorLog);
		std::cout << "Error Linking shader program " << errorLog << std::endl;
		return;
	}

	//validate();

	// 1. shader program itself
	// 2. search the program for the xMovement uniform variable
	// We want the id of the location of the uniform variable in the program
	uniformModel = glGetUniformLocation(m_shaderId, "model");
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "model error "
			<< error << std::endl;
	}
	errorReporter("Shader::compileProgram() u_model");

	uniformProjection = glGetUniformLocation(m_shaderId, "projection");
	errorReporter("Shader::compileProgram() u_projection");

	uniformView = glGetUniformLocation(m_shaderId, "view");
	errorReporter("Shader::compileProgram() u_view");

	uniformScaleMatrix = glGetUniformLocation(m_shaderId, "scaleMatrix");
	errorReporter("Shader::compileProgram() scaleMatrix");

	// Configuration the uniforms for the Directional Light
	uniformDirectionalLight.uniformColor = glGetUniformLocation(m_shaderId,
		"directionalLight.basicLight.color");

	uniformDirectionalLight.uniformAmbientIntensity = glGetUniformLocation(m_shaderId,
		"directionalLight.basicLight.ambientIntensity");

	uniformDirectionalLight.uniformDirection = glGetUniformLocation(m_shaderId,
		"directionalLight.direction");

	uniformDirectionalLight.uniformDiffuseIntensity = glGetUniformLocation(m_shaderId,
		"directionalLight.basicLight.diffuseIntensity");
	errorReporter("Shader::CompileProgram() binding directional light");

	//uniformColor = glGetUniformLocation(shader, "newColor");
	uniformSpecularIntensity = glGetUniformLocation(m_shaderId,
		"material.specularIntensity");
	uniformShininess = glGetUniformLocation(m_shaderId,
		"material.shininess");
	uniformEyePosition = glGetUniformLocation(m_shaderId,
		"eyePosition");

	uniformPointLightCount = glGetUniformLocation(m_shaderId,
		"pointLightCount");

	uniformSpotLightCount = glGetUniformLocation(m_shaderId,
		"spotLightCount");

	errorReporter("Shader::CompileProgram() binding light counts");

	// For the point lights
	for (int i = 0; i < MAX_POINT_LIGHTS; ++i) {
		char locationBuffer[99] = { '\0' };
		//locationBuffer = std::string();
		//locationBuffer = std::format("pointLights[{}].color", i);
		//locationBuffer[99] = { '\0' };
		snprintf(locationBuffer, sizeof(locationBuffer),
			"pointLights[%d].basicLight.color", i);
		uniformPointLight[i].uniformColor = glGetUniformLocation(m_shaderId, locationBuffer);

		snprintf(locationBuffer, sizeof(locationBuffer),
			"pointLights[%d].basicLight.ambientIntensity", i);
		uniformPointLight[i].uniformAmbientIntensity = glGetUniformLocation(m_shaderId, locationBuffer);

		snprintf(locationBuffer, sizeof(locationBuffer),
			"pointLights[%d].basicLight.diffuseIntensity", i);
		uniformPointLight[i].uniformDiffuseIntensity = glGetUniformLocation(m_shaderId, locationBuffer);

		snprintf(locationBuffer, sizeof(locationBuffer),
			"pointLights[%d].position", i);
		uniformPointLight[i].uniformPosition = glGetUniformLocation(m_shaderId, locationBuffer);

		snprintf(locationBuffer, sizeof(locationBuffer),
			"pointLights[%d].constant", i);
		uniformPointLight[i].uniformConstant = glGetUniformLocation(m_shaderId, locationBuffer);

		snprintf(locationBuffer, sizeof(locationBuffer),
			"pointLights[%d].linear", i);
		uniformPointLight[i].uniformLinear = glGetUniformLocation(m_shaderId, locationBuffer);

		snprintf(locationBuffer, sizeof(locationBuffer),
			"pointLights[%d].quadratic", i);
		uniformPointLight[i].uniformQuadratic = glGetUniformLocation(m_shaderId, locationBuffer);
	}

	errorReporter("Shader::CompileProgram() binding pointLight attr");

	// For the spot lights
	for (int i = 0; i < MAX_SPOT_LIGHTS; ++i) {
		char locationBuffer[99] = { '\0' };

		snprintf(locationBuffer, sizeof(locationBuffer),
			"spotLights[%d].basePointLight.basicLight.color", i);
		uniformSpotLight[i].uniformColor = glGetUniformLocation(m_shaderId, locationBuffer);

		snprintf(locationBuffer, sizeof(locationBuffer),
			"spotLights[%d].basePointLight.basicLight.ambientIntensity", i);
		uniformSpotLight[i].uniformAmbientIntensity = glGetUniformLocation(m_shaderId, locationBuffer);

		snprintf(locationBuffer, sizeof(locationBuffer),
			"spotLights[%d].basePointLight.basicLight.diffuseIntensity", i);
		uniformSpotLight[i].uniformDiffuseIntensity = glGetUniformLocation(m_shaderId, locationBuffer);

		snprintf(locationBuffer, sizeof(locationBuffer),
			"spotLights[%d].basePointLight.position", i);
		uniformSpotLight[i].uniformPosition = glGetUniformLocation(m_shaderId, locationBuffer);

		snprintf(locationBuffer, sizeof(locationBuffer),
			"spotLights[%d].basePointLight.constant", i);
		uniformSpotLight[i].uniformConstant = glGetUniformLocation(m_shaderId, locationBuffer);

		snprintf(locationBuffer, sizeof(locationBuffer),
			"spotLights[%d].basePointLight.linear", i);
		uniformSpotLight[i].uniformLinear = glGetUniformLocation(m_shaderId, locationBuffer);

		snprintf(locationBuffer, sizeof(locationBuffer),
			"spotLights[%d].basePointLight.quadratic", i);
		uniformSpotLight[i].uniformQuadratic = glGetUniformLocation(m_shaderId, locationBuffer);

		snprintf(locationBuffer, sizeof(locationBuffer),
			"spotLights[%d].direction", i);
		uniformSpotLight[i].uniformDirection = glGetUniformLocation(m_shaderId, locationBuffer);

		snprintf(locationBuffer, sizeof(locationBuffer),
			"spotLights[%d].edge", i);
		uniformSpotLight[i].uniformEdge = glGetUniformLocation(m_shaderId, locationBuffer);
	}

	errorReporter("Shader::CompileProgram() binding spotLight attr.");

	uniformTexture = glGetUniformLocation(m_shaderId, "diffuseTexture");

	uniformDirectionalLightTransform = glGetUniformLocation(m_shaderId,
		"directionalLightTransform");
	uniformDirectionalShadowMap = glGetUniformLocation(m_shaderId,
		"directionalShadowMap");

	errorReporter("Shader::CompileProgram() Error binding shadow map");

	uniformOmniLightPosition = glGetUniformLocation(m_shaderId,
		"lightPosition");
	uniformFarPlane = glGetUniformLocation(m_shaderId,
		"farPlane");


	for (int i = 0; i < 6; ++i) {
		char locationBuffer[99] = { '\0' };

		snprintf(locationBuffer, sizeof(locationBuffer),
			"lightMatrices[%d]", i);
		uniformLightMatrices[i] = glGetUniformLocation(m_shaderId,
			locationBuffer);
	}

	errorReporter("Shader::CompileProgram() at binding lightMatrices[]");

	for (int i = 0; i < MAX_POINT_LIGHTS + MAX_SPOT_LIGHTS; ++i) {
		char locationBuffer[99] = { '\0' };

		snprintf(locationBuffer, sizeof(locationBuffer),
			"omniShadowMaps[%d].shadowMap", i);
		uniformOmniShadowMap[i].shadowMap = glGetUniformLocation(m_shaderId,
			locationBuffer);

		snprintf(locationBuffer, sizeof(locationBuffer),
			"omniShadowMaps[%d].farPlane", i);
		uniformOmniShadowMap[i].farPlane = glGetUniformLocation(m_shaderId,
			locationBuffer);
	}

	errorReporter("Shader::CompileProgram() at binding uniformOmniShadowMap[]");
}

void Shader::compileDeferredProgram() {
	// error logging
	GLint result = 0;
	GLchar errorLog[1024] = { 0 };

	glLinkProgram(m_shaderId);
	glGetProgramiv(m_shaderId, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(m_shaderId, sizeof(errorLog), NULL, errorLog);
		std::cout << "Error Linking shader program " << errorLog << std::endl;
		return;
	}

	uniformModel = glGetUniformLocation(m_shaderId, "u_model");
	uniformProjection = glGetUniformLocation(m_shaderId, "u_projection");
	uniformView = glGetUniformLocation(m_shaderId, "u_view");
	errorReporter("Shader::compileProgram() matrices");

	m_uDiffuseTexture = glGetUniformLocation(m_shaderId, "texture_diffuse1");
	m_uSpecularTexture = glGetUniformLocation(m_shaderId, "texture_specular1");
	errorReporter("Shader::compileProgram() textures");
}

void Shader::addShader(
	GLuint programId,
	std::string shaderCode,
	GLenum shaderType
) {
	// create the individual shader
	// create an empty shader for that type
	GLuint shader = glCreateShader(shaderType);

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
	glShaderSource(shader, 1, shaderCodePointer, shaderCodeStrLength);
	glCompileShader(shader);

	// Before we attach it to the program itself, we need to do more error checking
	GLint result = 0;
	GLchar errorLog[1024] = { 0 };
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(m_shaderId, sizeof(errorLog), NULL, errorLog);
		std::cout << "Error compiling shader type: "
			<< shaderType << " with error: "
			<< errorLog << std::endl;
		return;
	}

	// Now that everything is compiled we can attach to the program
	glAttachShader(programId, shader);
}

