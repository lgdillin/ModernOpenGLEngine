#pragma once

#include <iostream>

#include <GL/glew.h>

class Quad {
public:
	Quad();
	~Quad();

	void initialize(unsigned int _width, unsigned int _height);

	void render();

private:
	GLuint m_vertexArrayObject;
	GLuint m_indexBufferObject;
	GLuint m_vertexBufferObject;

	unsigned int m_width, m_height;

	static const GLuint m_vertexArraySize = 44;
	static const GLuint m_indicesArraySize = 6;
	GLfloat m_vertices[m_vertexArraySize] = {
		// Bottom left
		-1.0f, -1.0f, 0.0f,// x,y,z
		1.0f, 1.0f, 1.0f, // r,g,b
		0.0f, 0.0f, // s,t
		0.0f, 0.0f, 1.0f,

		// Bottom Right
		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,

		// Top Right
		1.0f,  1.0f, 0.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		// Top Left
		-1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
	};

	unsigned int m_indices[m_indicesArraySize] = {
		0, 1, 2,
		2, 3, 0
	};
};