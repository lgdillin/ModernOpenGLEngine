#include "Mesh.hpp"


Mesh::Mesh() {
	vertexArrayObject = 0;
	vertexBufferObject = 0;
	indexBufferObject = 0;
	indexArraySize = 0;
}

Mesh::~Mesh() {
	clearMesh();
}

void Mesh::createMesh(
	GLfloat* vertices, // vertex data
	unsigned int* indices, // ordinal data for constructing triangles
	unsigned int vertexArraySize,
	unsigned int indexArraySize
) {
	this->indexArraySize = indexArraySize;

	// Request a VAO id (an alloc memory) from the GPU and 
	// save it in the variable (generate the VAO names)
	glGenVertexArrays(1, &vertexArrayObject);
	// Bind the vertex array to the ID generated (So we can access the memory)
	glBindVertexArray(vertexArrayObject);

	// Generate buffer ID for the IBO/EBO (which stores the ordinal data
	// for the indices
	glGenBuffers(1, &indexBufferObject);
	// Bind this ID to the ELEMENT_ARRAY_BUFFER (Vertex array indices)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
	// Create and initialize the IBO's data store
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
		sizeof(indices[0]) * indexArraySize, indices, GL_STATIC_DRAW);


	// generate the buffer object name (read: ID) for the VBO
	glGenBuffers(1, &vertexBufferObject);
	// bind the named buffer object to the GL_ARRAY_BUFFER (vertex attributes
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	// Create and initialize the VBO's data store
	glBufferData(GL_ARRAY_BUFFER, 
		sizeof(vertices[0]) * vertexArraySize, vertices, GL_STATIC_DRAW);


	// Right now the format for vertex data is stored as:
	// {x1,y1,z1,r1,g1,b1,x2,y2,z2,r2,g2,b2, ...}
	// define the layout of the (x,y,z) portion of the vertex attribute data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		6 * sizeof(GLfloat), (GLvoid*)0);

	// define the layout of the (r,g,b) portion of the vertex attribute data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

	// we want to enable that location=0 in the VS
	glEnableVertexAttribArray(0);
	// Also enable the location=1 in the VS
	glEnableVertexAttribArray(1);

	// Unbinding the VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind the IBO/EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	// Unbind the VAO
	glBindVertexArray(0);
}

void Mesh::renderMesh() {
	// Use the shader we have bound
	glBindVertexArray(vertexArrayObject);

	// Bind the IBO id to the target buffer GL_ELEMENT_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);

	// Draw our elements
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

	// Unbind the GL_ELEMENT_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	// Unbind our Vertex Array
	glBindVertexArray(0);
}

// Clears the mesh without deleting the object
void Mesh::clearMesh() {
	if (vertexBufferObject != 0) {
		glDeleteBuffers(1, &vertexBufferObject);
		vertexBufferObject = 0;
	}

	if (indexBufferObject != 0) {
		glDeleteBuffers(1, &indexBufferObject);
		indexBufferObject = 0;
	}

	if (vertexArrayObject != 0) {
		glDeleteVertexArrays(1, &vertexArrayObject);
		vertexArrayObject = 0;
	}

	indexArraySize = 0;
}

// This function maps over the color data portion for one vertex
// using the glMapBufferRange() function
// (May be expensive to call multiple times)

// NOTE: we are bastardizing the argument right now 
void Mesh::mapBufferRangeVertexColorData(unsigned int index) {
	// Since we want to modify the data of an individual vertex,
	// We should bind to the GL_ARRAY_BUFFER with our Vertex Buffer Object
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

	GLsizeiptr rangeSize = 24 * sizeof(GLfloat);
	GLintptr offset = 0;
	//GLbitfield access = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT;
	GLbitfield access = GL_MAP_WRITE_BIT;
	float* mappedData = static_cast<float*>(
		glMapBufferRange(GL_ARRAY_BUFFER, 0, rangeSize, access));

	// Do the modification
	// Right now we aren't using the function arguments 
	//mappedData[3] = abs(sin(incrementArgument * toRadians));
	//mappedData[4] = abs(-sin(incrementArgument * toRadians - 1));

	//mappedData[10] = abs(sin(incrementArgument * toRadians - 1));
	//mappedData[11] = abs(-sin(incrementArgument * toRadians - 3));

	//mappedData[17] = abs(sin(incrementArgument * toRadians - 3));
	//mappedData[16] = abs(-sin(incrementArgument * toRadians - 1));

	//mappedData[22] = abs(sin(incrementArgument * toRadians - 5));
	//mappedData[23] = abs(-sin(incrementArgument * toRadians - 3));

	if (!glUnmapBuffer(GL_ARRAY_BUFFER)) {
		std::cout << "it failed" << std::endl;
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}