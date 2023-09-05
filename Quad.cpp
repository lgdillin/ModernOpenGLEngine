#include "Quad.hpp"

Quad::Quad() {
	m_vertexArrayObject = 0;
}

Quad::~Quad() {
}

void Quad::initialize(unsigned int _width, unsigned int _height) {
	glGenVertexArrays(1, &m_vertexArrayObject);
	glBindVertexArray(m_vertexArrayObject);

	glGenBuffers(1, &m_indexBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_indices[0]) * m_indicesArraySize,
		m_indices, GL_STATIC_DRAW);

	glGenBuffers(1, &m_vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices[0]) * m_vertexArraySize,
		m_vertices, GL_STATIC_DRAW);

	// X, Y coords
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), 
		(GLvoid *)0);
	// R, G, B coords
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), 
		(GLvoid *)(3 * sizeof(GLfloat)));
	// S, T coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), 
		(GLvoid *)(6 * sizeof(GLfloat)));
	// normal coords
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat),
		(GLvoid *)(8 * sizeof(GLfloat)));


	// we want to enable that location=0 in the VS
	glEnableVertexAttribArray(0);
	// Also enable the location=1 in the VS
	glEnableVertexAttribArray(1);
	// location=2 is for the texture
	glEnableVertexAttribArray(2);
	// location=2 is for the texture
	glEnableVertexAttribArray(3);

	// Unbinding the VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind the IBO/EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	// Unbind the VAO
	glBindVertexArray(0);
}

void Quad::render() {
	glBindVertexArray(m_vertexArrayObject);

	// Bind the IBO id to the target buffer GL_ELEMENT_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferObject);

	// Draw our elements
	glDrawElements(GL_TRIANGLES, m_indicesArraySize, GL_UNSIGNED_INT, 0);

	// Unbind the GL_ELEMENT_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	// Unbind our Vertex Array
	glBindVertexArray(0);
}
