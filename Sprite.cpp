#include "Sprite.hpp"

Sprite::Sprite() {
	m_position = glm::vec3(0.0f);
	m_color = glm::vec3(1.0f);
	m_projection = glm::ortho(0.0f, 50.0f, 50.0f, 0.0f, -1.0f, 1.0f);
}

Sprite::Sprite(
	glm::vec3 _position,
	std::string _fileLocation,
	glm::vec3 _color
) : Texture(_fileLocation) {
	m_position = _position;
	m_color = _color;
	m_projection = glm::ortho(0.0f, 50.0f, 50.0f, 0.0f, -1.0f, 1.0f);
}

Sprite::~Sprite() {
}

void Sprite::initialize() {
	unsigned int vertexBufferObject;
	float vertices[] = {
		// pos      // tex
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &m_vertexArrayObject);
	glGenBuffers(1, &vertexBufferObject);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), 
		vertices, GL_STATIC_DRAW);

	glBindVertexArray(m_vertexArrayObject);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 
		4 * sizeof(float), (void *)0);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Sprite::render() {

	// use a shader

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, m_position);

	useTexture();
	
}
