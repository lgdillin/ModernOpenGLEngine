#include "Mesh.hpp"


Mesh::Mesh() {
	vertexArrayObject = 0;
	vertexBufferObject = 0;
	indexBufferObject = 0;
	indexArraySize = 0;
	vertexArraySize = 0;
	srand((unsigned)time(NULL));
}

Mesh::~Mesh() {
	clearMesh();
}

void Mesh::createMesh(
	GLfloat* _vertices, // vertex data
	unsigned int* _indices, // ordinal data for constructing triangles
	unsigned int vArraySize,
	unsigned int iArraySize
) {
	vertexArraySize = vArraySize;
	indexArraySize = iArraySize;
	vertices = _vertices;
	indices = _indices;


	// Sometimes the VAO doesn't generate?
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << error << std::endl;
	}


	// Request a VAO id (an alloc memory) from the GPU and 
	// save it in the variable (generate the VAO names)
	glGenVertexArrays(1, &vertexArrayObject);
	

	// Bind the vertex array to the ID generated (So we can access the memory)
	glBindVertexArray(vertexArrayObject);

	// Generate buffer ID for the IBO/EBO (which stores the ordinal data
	// for the INDICES)
	glGenBuffers(1, &indexBufferObject);
	// Bind this ID to the ELEMENT_ARRAY_BUFFER (Vertex array indices)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
	// Create and initialize the IBO's data store
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
		sizeof(indices[0]) * indexArraySize, indices, GL_STATIC_DRAW);


	// generate the buffer object name (read: ID) for the VBO
	// This stores all of the VERTICES
	glGenBuffers(1, &vertexBufferObject);
	// bind the named buffer object to the GL_ARRAY_BUFFER (vertex attributes
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	// Create and initialize the VBO's data store
	glBufferData(GL_ARRAY_BUFFER, 
		sizeof(vertices[0]) * vertexArraySize, vertices, GL_STATIC_DRAW);


	// Right now the format for vertex data is stored as:
	// {x1,y1,z1,r1,g1,b1,s1,t1,x2,y2,z2,r2,g2,b2,s2,t2, ...}
	// define the layout of the (x,y,z) portion of the vertex attribute data
	// If we have a vertex with 8 total data points {x,y,z,r,g,b,s,t]
	// Then we need to specify that value to the stride, since that is effectively
	// the gap between each relevant 'stripe' of data for the relevant pointer
	// However, we will need to identify the offset position for each vertex unit
	// for the *pointer argument. Since the texture coordinates for each vertex
	// with index [0,7] is 6 and 7, we will need to specify that
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		8 * sizeof(GLfloat), (GLvoid*)0);

	// define the layout of the (r,g,b) portion of the vertex attribute data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

	// define the layout of the (s,t) texture portion of the vertex attribute data
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
		8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));

	// we want to enable that location=0 in the VS
	glEnableVertexAttribArray(0);
	// Also enable the location=1 in the VS
	glEnableVertexAttribArray(1);
	// We will probably need a location=2
	glEnableVertexAttribArray(2);

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
	glDrawElements(GL_TRIANGLES, indexArraySize, GL_UNSIGNED_INT, 0);

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
void Mesh::mapBufferRangeVertexColorData() {
	// Since we want to modify the data of an individual vertex,
	// We should bind to the GL_ARRAY_BUFFER with our Vertex Buffer Object
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

	GLsizeiptr rangeSize = vertexArraySize * sizeof(GLfloat);
	GLintptr offset = 0;
	//GLbitfield access = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT;
	GLbitfield access = GL_MAP_WRITE_BIT;
	float* mappedData = static_cast<float*>(
		glMapBufferRange(GL_ARRAY_BUFFER, 0, rangeSize, access));

	//for (int i = 0; i < vertexArraySize / 8; ++i) {
	//	//mappedData[i * 8] = abs(glm::sin(glm::radians(funVariable)));
	//	//mappedData[i * 8 + 1] = abs(glm::sin(glm::radians(funVariable - 1)));
	//	//mappedData[i * 8 + 2] = abs(glm::sin(glm::radians(funVariable - 3)));
	//
	//	mappedData[i * 8 + 3] = abs(0.1f * glm::sin(
	//		glm::radians(funVariable)));
	//	mappedData[i * 8 + 4] = abs(0.5f * glm::sin(
	//		glm::radians(funVariable)));
	//	mappedData[i * 8 + 5] = abs(0.1f * glm::sin(
	//		glm::radians(funVariable)));
	//}
	++funVariable;

	mappedData[3] = abs(glm::sin(0.1f * glm::radians(funVariable)));

	mappedData[12] = abs(glm::sin(0.1f * glm::radians(funVariable) - 1 ));

	mappedData[21] = abs(glm::sin(0.1f * glm::radians(funVariable) - 3));

	mappedData[29] = abs(glm::sin(0.1f * glm::radians(funVariable) - 5));
	mappedData[28] = abs(glm::sin(0.1f * glm::radians(funVariable) - 5));

	mappedData[35] = abs(glm::sin(0.1f * glm::radians(funVariable) - 3.5f));
	mappedData[37] = abs(glm::sin(0.1f * glm::radians(funVariable) - 3.5f));
	
	mappedData[45] = abs(glm::sin(0.1f * glm::radians(funVariable) - 1.2f));
	mappedData[45] = abs(glm::sin(0.1f * glm::radians(funVariable) - 1.2f));
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