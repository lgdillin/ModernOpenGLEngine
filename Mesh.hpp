#pragma once

#include <iostream> 
#include <GL/glew.h>

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
	
	
	void renderMesh();
	void clearMesh(); // Clears the data without deleting object
	void mapBufferRangeVertexColorData(unsigned int index);

private:
	GLuint vertexArrayObject,
		vertexBufferObject, 
		indexBufferObject;

	GLsizei indexArraySize;
};

