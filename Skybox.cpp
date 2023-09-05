#include "Skybox.hpp"

Skybox::Skybox() {
}

Skybox::Skybox(std::vector<std::string> _faceLocations) {

	// Shader setup
	m_skyboxShader = new Shader();
	m_skyboxShader->createFromFile(
		"skyboxShader.vert",
		"skyboxShader.frag",
		Shader::FOWARD_SHADING
	);

	m_uniformProjection = m_skyboxShader->getProjectionLocation();
	m_uniformView = m_skyboxShader->getViewLocation();

	// Texture Setup
	glGenTextures(1, &m_textureId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureId);

	int width, height, bitDepth;
	for (int i = 0; i < 6; ++i) {
		unsigned char *textureData = stbi_load(_faceLocations[i].c_str(),
			&width, &height, &bitDepth, 0);

		if (!textureData) {
			std::cout << "Skybox::Skybox() loading file: " 
				<< _faceLocations[i]
				<< " check location or mode" << std::endl;
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
			width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);

		stbi_image_free(textureData);
	}

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "Skybox::Skybox " << error
			<< " when loading textures" << std::endl;
	}

	// Handle texture wrapping on the S and T axis (read: X and Y axis)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Manage the scaling of the texture's look when very close/far
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "Skybox::Skybox " << error
			<< " when configuring texture parameters" << std::endl;
	}

	// Mesh Setup
	m_skyboxMesh = new Mesh();
	GLfloat vertices[] = {
	-1.0f, 1.0f, -1.0f,		0.0f, 0.0f, 0.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
	-1.0f, -1.0f, -1.0f,	0.0f, 0.0f, 0.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
	1.0f, 1.0f, -1.0f,		0.0f, 0.0f, 0.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
	1.0f, -1.0f, -1.0f,		0.0f, 0.0f, 0.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,

	-1.0f, 1.0f, 1.0f,		0.0f, 0.0f, 0.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
	1.0f, 1.0f, 1.0f,		0.0f, 0.0f, 0.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
	-1.0f, -1.0f, 1.0f,		0.0f, 0.0f, 0.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
	1.0f, -1.0f, 1.0f,		0.0f, 0.0f, 0.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int indices[] = {
		// front
		0, 1, 2,
		2, 1, 3,
		// right
		2, 3, 5,
		5, 3, 7,
		// back
		5, 7, 4,
		4, 7, 6,
		// left
		4, 6, 0,
		0, 6, 1,
		// top
		4, 0, 5,
		5, 0, 2,
		// bottom
		1, 6, 3,
		3, 6, 7
	};

	m_skyboxMesh->createMesh(vertices,
		indices, 88, 36);

	error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "Skybox::Skybox " << error
			<< " when building mesh" << std::endl;
	}
}

Skybox::Skybox(std::string *_faceLocations, int _faces) {
	// Shader setup
	m_skyboxShader = new Shader();
	m_skyboxShader->createFromFile(
		"skyboxShader.vert",
		"skyboxShader.frag",
		Shader::FOWARD_SHADING
	);

	m_uniformProjection = m_skyboxShader->getProjectionLocation();
	m_uniformView = m_skyboxShader->getViewLocation();

	// Texture Setup
	glGenTextures(1, &m_textureId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureId);

	int width, height, bitDepth;
	for (int i = 0; i < _faces; ++i) {
		unsigned char *textureData = stbi_load(_faceLocations[i].c_str(),
			&width, &height, &bitDepth, 0);

		if (!textureData) {
			std::cout << "Skybox::Skybox() loading file: "
				<< _faceLocations[i]
				<< " check location or mode" << std::endl;
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
			width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);

		stbi_image_free(textureData);
	}

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "Skybox::Skybox " << error
			<< " when loading textures" << std::endl;
	}

	// Handle texture wrapping on the S and T axis (read: X and Y axis)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Manage the scaling of the texture's look when very close/far
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "Skybox::Skybox " << error
			<< " when configuring texture parameters" << std::endl;
	}

	// Mesh Setup
	m_skyboxMesh = new Mesh();
	GLfloat vertices[] = {
	-1.0f, 1.0f, -1.0f,		0.0f, 0.0f, 0.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
	-1.0f, -1.0f, -1.0f,	0.0f, 0.0f, 0.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
	1.0f, 1.0f, -1.0f,		0.0f, 0.0f, 0.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
	1.0f, -1.0f, -1.0f,		0.0f, 0.0f, 0.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,

	-1.0f, 1.0f, 1.0f,		0.0f, 0.0f, 0.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
	1.0f, 1.0f, 1.0f,		0.0f, 0.0f, 0.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
	-1.0f, -1.0f, 1.0f,		0.0f, 0.0f, 0.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
	1.0f, -1.0f, 1.0f,		0.0f, 0.0f, 0.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int indices[] = {
		// front
		0, 1, 2,
		2, 1, 3,
		// right
		2, 3, 5,
		5, 3, 7,
		// back
		5, 7, 4,
		4, 7, 6,
		// left
		4, 6, 0,
		0, 6, 1,
		// top
		4, 0, 5,
		5, 0, 2,
		// bottom
		1, 6, 3,
		3, 6, 7
	};

	m_skyboxMesh->createMesh(vertices,
		indices, 88, 36);

	error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "Skybox::Skybox " << error
			<< " when building mesh" << std::endl;
	}
}

Skybox::~Skybox() {
	// destroy!
}

void Skybox::drawSkybox(
	glm::mat4 _viewMatrix, 
	glm::mat4 _projectionMatrix
) {
	// convert the view matrix to a 3x3 matrix, 
	// which will cut off translation, but keep rotation and scaling
	_viewMatrix = glm::mat4(glm::mat3(_viewMatrix));

	glDepthMask(GL_FALSE); // Disable prior to skybox drawing
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "Skybox::drawSkybox() " << error
			<< " when disabling depthMask" << std::endl;
	}

	m_skyboxShader->useShader();

	glUniformMatrix4fv(m_uniformView, 1, GL_FALSE,
		glm::value_ptr(_viewMatrix));
	glUniformMatrix4fv(m_uniformProjection, 1, GL_FALSE, 
		glm::value_ptr(_projectionMatrix));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureId);

	// validate as a form of bug-checking
	m_skyboxShader->validate();

	m_skyboxMesh->renderMesh();

	glDepthMask(GL_TRUE); // renable when done with skybox
}

void Skybox::generateVerticesAndIndices() {
	m_skyboxVertices = {
		-1.0f, 1.0f, -1.0f,		0.0f, 0.0f, 0.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, -1.0f,	0.0f, 0.0f, 0.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, -1.0f,		0.0f, 0.0f, 0.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, -1.0f,		0.0f, 0.0f, 0.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,

		-1.0f, 1.0f, 1.0f,		0.0f, 0.0f, 0.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f,		0.0f, 0.0f, 0.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, 1.0f,		0.0f, 0.0f, 0.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f,		0.0f, 0.0f, 0.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f
	};

	m_skyboxIndices = {
		// front
		0, 1, 2,
		2, 1, 3,
		// right
		2, 3, 5,
		5, 3, 7,
		// back
		5, 7, 4,
		4, 7, 6,
		// left
		4, 6, 0,
		0, 6, 1,
		// top
		4, 0, 5,
		5, 0, 2,
		// bottom
		1, 6, 3,
		3, 6, 7
	};
}
