#pragma once

#include <iostream> 
#include <stdlib.h>
#include <GL/glew.h>

#include <glm/glm.hpp>

class Mesh
{
public:
	Mesh();
	~Mesh();

	void createMesh(
		GLfloat* vertices, // vertex data
		unsigned int* indices, // ordinal data for constructing triangles
		unsigned int vertexArraySize,
		unsigned int indexArraySize
	);
	
	GLuint* vao() { return &vertexArrayObject; }
	GLfloat* ptr() { return vertices; }
	
	void renderMesh();
	void clearMesh(); // Clears the data without deleting object
	void mapBufferRangeVertexColorData();

private:
	GLfloat funVariable;
	GLuint vertexArrayObject,
		vertexBufferObject, 
		indexBufferObject;
		
	GLfloat* vertices;
	unsigned int* indices;

	GLsizei indexArraySize, vertexArraySize;
};

