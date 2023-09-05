#include "RectangularPrism.hpp"

RectangularPrism::RectangularPrism() {
	vertexArraySize = vertexDataLength;
	indexArraySize = indicesDataLength;

	rotationAngle = 0.0f;
	translationVector = glm::vec3(1.0f, 1.0f, 1.0f);
	rotationVector = glm::vec3(0.0f, 1.0f, 0.0f);
	scalingVector = glm::vec3(1.0f, 1.0f, 1.0f);

	//vertices = nullptr;
	//indices = nullptr;

	model = glm::mat4(1);
	scale = glm::mat4(1);
	texture = nullptr;
	mesh = nullptr;

	vertices = { 1.0f };

	//boundingBox[24] = { 0 };
	//immutableBoundingBox[24] = { 0 };

	xMinBound = 0;
	xMaxBound = 0;
	yMinBound = 0;
	yMaxBound = 0;
	zMinBound = 0;
	zMaxBound = 0;
}

RectangularPrism::~RectangularPrism() {
	//delete[] vertices;
	//delete[] indices;
}

void RectangularPrism::initialize(std::string pickColor) {
	buildVertexArray1();
	buildIndexArray1();

	computeInterpolatedNormalsSmoothEdges();

	//pickColor("red");

	mesh = new Mesh();
	initMesh();

	model = glm::mat4(1);

	initBoundingBox();

}

void RectangularPrism::transformBoundingBox(glm::mat4 model) {
	// translate the box
	for (int i = 0; i < 8; ++i) {
		glm::vec4 vector = glm::vec4(
			immutableBoundingBox[i * 3],
			immutableBoundingBox[i * 3 + 1],
			immutableBoundingBox[i * 3 + 2], 1.0f
		);

		glm::vec4 transformedVec = model * vector;

		boundingBox[i * 3] = transformedVec.x;
		boundingBox[i * 3 + 1] = transformedVec.y;
		boundingBox[i * 3 + 2] = transformedVec.z;
	}
	//updateBounds();
}

void RectangularPrism::transform() {
	model = glm::mat4(1);

	model = glm::translate(model, translationVector);
	transformBoundingBox(model);
	model = glm::rotate(model, rotationAngle, rotationVector);
	transformBoundingBox(model);
	model = glm::scale(model, scalingVector);
	transformBoundingBox(model);
	updateBounds();
}

glm::mat4 RectangularPrism::scaleTexture() {
	scale = glm::mat4(1);
	//scale = glm::scale(scale, scalingVector);
	return scale;
}

void RectangularPrism::initMesh() {
	mesh->createMesh(vertices.data(), indices.data(), vertexArraySize, indexArraySize);
}

void RectangularPrism::computeInterpolatedNormalsSmoothEdges() {
	unsigned int index0, index1, index2;
	for (size_t i = 0; i < indexArraySize / 3; ++i) {

		// Cycle through every triangle defined in our Index Array
		// Store the index data in index0, index1, and index2
		// The purpose of this code is to grab the actual vertex values that
		// exist at each one of these indices, and use the value of the index
		// to map to adjacent vertices in the vertex data
		index0 = indices[i * 3] * numberOfVertexComponents;
		index1 = indices[i * 3 + 1] * numberOfVertexComponents;
		index2 = indices[i * 3 + 2] * numberOfVertexComponents;

		
		// we use the points from above, and connect two together to create
		// a line, and then connect a seperate two points to create a line,
		// and then compute the cross product on those two lines (vectors)
		// to get a vector which is perpendicular to the two

		// Suppose we have 2 vectors. the vector 'created' from index1:
		// vec3(index1, index1+1, index1+2) and index 0.
		// vec3(index0, index0+1, index0+2).
		// by subtracting V_index1 - V_index0, we get a vector
		// that points from the tip of V_index0 to V_index1
		// (Essentially this means we are subtracting the B component
		// from the combined components of vectors A+B)  
		// Do the same for subtracting V_index0 from V_index2
		glm::vec3 v1(
			vertices[index1] - vertices[index0],
			vertices[index1 + 1] - vertices[index0 + 2],
			vertices[index1 + 2] - vertices[index0 + 2]);

		glm::vec3 v2(
			vertices[index2] - vertices[index0],
			vertices[index2 + 1] - vertices[index0 + 2],
			vertices[index2 + 2] - vertices[index0 + 2]);

		// Now that both vectors V_index1 and V_index2 no longer have 
		// any shared V_index data. Then we produce the cross product,
		// giving us a vector perpindicular to the two vectors, 
		// and normalize it.
		glm::vec3 normal = glm::normalize(glm::cross(v1, v2));

		// Add the index offset for where the normal data is stored
		// in each vertex within the vertices array
		index0 += offsetForVertexNormals;
		index1 += offsetForVertexNormals;
		index2 += offsetForVertexNormals;

		// Copy the data over, writing our interpolated normalized normals
		// To their relevant location in the vertex data
		vertices[index0] += normal.x;
		vertices[index0 + 1] += normal.y;
		vertices[index0 + 2] += normal.z;

		vertices[index1] += normal.x;
		vertices[index1 + 1] += normal.y;
		vertices[index1 + 2] += normal.z;

		vertices[index2] += normal.x;
		vertices[index2 + 1] += normal.y;
		vertices[index2 + 2] += normal.z;
	}

	unsigned int nOffset;
	for (size_t i = 0; i < vertexArraySize / numberOfVertexComponents; ++i) {

		// Copy a vector from out Vertex Data and normalize it
		nOffset = i * numberOfVertexComponents + offsetForVertexNormals;
		glm::vec3 offsetVector = glm::normalize(glm::vec3(
			vertices[nOffset],
			vertices[nOffset + 1],
			vertices[nOffset + 2]));

		// Replace the vector values inside the Vertex Data with
		// our new normalized values
		vertices[nOffset] = offsetVector.x;
		vertices[nOffset + 1] = offsetVector.y;
		vertices[nOffset + 2] = offsetVector.z;

	}
}

void RectangularPrism::draw(Shader &_shader) {
	// assign active texture
	unsigned int textureUnitOffset = 1;
	//glActiveTexture(GL_TEXTURE0 + textureUnitOffset);

	// assign texture type (currently hardcoded)
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(_shader.getM_shaderId(),
		"texture_diffuse1"), 0);
	glBindTexture(GL_TEXTURE_2D, texture->getTextureId());

	glActiveTexture(GL_TEXTURE1);
	glUniform1i(glGetUniformLocation(_shader.getM_shaderId(),
		"texture_specular1"), 1);
	glBindTexture(GL_TEXTURE_2D, m_specularMap->getTextureId());

	renderMesh();

	glActiveTexture(GL_TEXTURE0);
}

glm::vec3 RectangularPrism::getBBVertex(int index) {
	return glm::vec3(
		boundingBox[3 * index],
		boundingBox[3 * index + 1],
		boundingBox[3 * index + 2]
	);
}

void RectangularPrism::swapColors(glm::vec3 color) {
	// each vertex has 8 components, and we want components 3,4,5
	int limiter = vertexArraySize / numberOfVertexComponents;
	for (int i = 0; i < limiter; ++i) {
		vertices[i * numberOfVertexComponents + 3] = color.x;
		vertices[i * numberOfVertexComponents + 4] = color.y;
		vertices[i * numberOfVertexComponents + 5] = color.z;
	}
}

// sticking with default values is fine, but i wanted some fun
void RectangularPrism::pickColor(
	std::string color,
	glm::vec3 customColor) {
	if (color == "red") {
		swapColors(glm::vec3(1.0f, 0.0f, 0.0f));
	} else if (color == "green") {
		swapColors(glm::vec3(0.0f, 1.0f, 0.0f));
	} else if (color == "blue") {
		swapColors(glm::vec3(0.0f, 0.0f, 1.0f));
	} else if(color == "white") {
		swapColors(glm::vec3(1.0f, 1.0f, 1.0f));
	} else if (color == "custom") {
		swapColors(customColor);
	}
}

void RectangularPrism::addTextureFromFile(std::string textureFileString) {
	//texture = Texture(textureFileString);
	//texture.loadTexture();
}

void RectangularPrism::printVertexData() {
	std::cout << std::endl << "Contents of Vertex Data: " << std::endl;
	for (int i = 0; i < vertexArraySize / numberOfVertexComponents; ++i) {
		int strider = i * numberOfVertexComponents;
		
		std::cout
			<< "("
			<< vertices[strider] << ", "
			<< vertices[strider + 1] << ", "
			<< vertices[strider + 2]
			<< ") \t ("
			<< vertices[strider + 3] << ", "
			<< vertices[strider + 4] << ", "
			<< vertices[strider + 5]
			<< ") \t ("
			<< vertices[strider + 6] << ", "
			<< vertices[strider + 7]
			<< ") \t ("
			<< vertices[strider + 8] << ", "
			<< vertices[strider + 9] << ", "
			<< vertices[strider + 10]
			<< ")";
		std::cout << std::endl;
	}
}

void RectangularPrism::printIndexData() {
	std::cout << std::endl << "Contents of Index Data: " << std::endl;
	for (int i = 0; i < indexArraySize; ++i) {
		if (i % 3 == 0) std::cout << std::endl;
		std::cout << indices[i];
		std::cout << ", ";
	}
}

void RectangularPrism::updateBounds() {
	xMinBound = boundingBox[0];
	xMaxBound = boundingBox[0];

	yMinBound = boundingBox[1];
	yMaxBound = boundingBox[1];

	zMinBound = boundingBox[2];
	zMaxBound = boundingBox[2];

	for (int i = 0; i < 8; ++i) {
		if (boundingBox[i * 3] < xMinBound) xMinBound = boundingBox[i * 3];
		if (boundingBox[i * 3] > xMaxBound) xMaxBound = boundingBox[i * 3];

		if (boundingBox[i * 3 + 1] < yMinBound) yMinBound = boundingBox[i * 3 + 1];
		if (boundingBox[i * 3 + 1] > yMaxBound) yMaxBound = boundingBox[i * 3 + 1];

		if (boundingBox[i * 3 + 2] < zMinBound) zMinBound = boundingBox[i * 3 + 2];
		if (boundingBox[i * 3 + 2] > zMaxBound) zMaxBound = boundingBox[i * 3 + 2];
	}
	//std::cout << "";
}

void RectangularPrism::initBoundingBox() {

	GLfloat xMin = vertices[0];
	GLfloat xMax = vertices[0];

	GLfloat yMin = vertices[1];
	GLfloat yMax = vertices[1];

	GLfloat zMin = vertices[2];
	GLfloat zMax = vertices[2];
	for (int i = 0; i < vertexDataLength / numberOfVertexComponents; ++i) {
		if (vertices[numberOfVertexComponents * i] < xMin) xMin = vertices[numberOfVertexComponents * i];
		if (vertices[numberOfVertexComponents * i] > xMax) xMax = vertices[numberOfVertexComponents * i];

		if (vertices[numberOfVertexComponents * i + 1] < yMin) yMin = vertices[numberOfVertexComponents * i + 1];
		if (vertices[numberOfVertexComponents * i + 1] > yMax) yMax = vertices[numberOfVertexComponents * i + 1];

		if (vertices[numberOfVertexComponents * i + 2] < zMin) zMin = vertices[numberOfVertexComponents * i + 2];
		if (vertices[numberOfVertexComponents * i + 2] > zMax) zMax = vertices[numberOfVertexComponents * i + 2];
	}

	// vertices are defined front to back, CCW starting bottom left
	// vertex 0:
	immutableBoundingBox[0] = xMin;
	immutableBoundingBox[1] = yMin;
	immutableBoundingBox[2] = zMax;

	// vertex 1;:
	immutableBoundingBox[3] = xMax;
	immutableBoundingBox[4] = yMin;
	immutableBoundingBox[5] = zMax;

	//vertex 2
	immutableBoundingBox[6] = xMax;
	immutableBoundingBox[7] = yMax;
	immutableBoundingBox[8] = zMax;

	// vertex 3
	immutableBoundingBox[9] = xMin;
	immutableBoundingBox[10] = yMax;
	immutableBoundingBox[11] = zMax;

	// vertex 4
	immutableBoundingBox[12] = xMin;
	immutableBoundingBox[13] = yMin;
	immutableBoundingBox[14] = zMin;

	// vertex 5
	immutableBoundingBox[15] = xMax;
	immutableBoundingBox[16] = yMin;
	immutableBoundingBox[17] = zMin;

	// vertex 6
	immutableBoundingBox[18] = xMax;
	immutableBoundingBox[19] = yMax;
	immutableBoundingBox[20] = zMin;

	// vertex 7
	immutableBoundingBox[21] = xMin;
	immutableBoundingBox[22] = yMax;
	immutableBoundingBox[23] = zMin;
}

void RectangularPrism::buildVertexArray1() {
	vertices = {
		// Front face
			// 0F: front bottom left
			-0.5f, -0.5f, 0.5f, // x, y, z
			1.0f, 0.0f, 0.0f, // r, g, b
			0.0f, 0.0f, // s, t
			0.0f, 0.0f, 1.0f, // Normals nx, ny, nz

			// 1F: Bottom Front Right
			0.5f, -0.5f, 0.5f,
			0.0f, 1.0f, 0.0f,
			1.0f, 0.0f,
			0.0f, 0.0f, 1.0f,

			// 2F: Front Top Right
			0.5f, 0.5f, 0.5f,
			0.0f, 0.0f, 1.0f,
			1.0f, 1.0f,
			0.0f, 0.0f, 1.0f,

			// 3F: Front Top Left
			-0.5f, 0.5f, 0.5f,
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,

		// Back Face
			// 4B: Back Bottom Left
			-0.5f, -0.5f, -0.5f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 0.0f, -1.0f,

			// 5B: Back Bottom Right 
			0.5f, -0.5f, -0.5f,
			0.0f, 0.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f, -1.0f,

			// 6B: Back Top Right 
			0.5f, 0.5f, -0.5f,
			0.0f, 1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 0.0f, -1.0f,

			// 7B: Back Top Left
			-0.5f, 0.5f, -0.5f,
			0.0f, 0.0f, 1.0f,
			0.0f, 1.0f,
			0.0f, 0.0f, -1.0f,
		
		// Left face
			// 8L: Left Bottom Left
			-0.5f, -0.5f, -0.5f,
			1.0f, 0.0f, 1.0f,
			0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,

			// 9L: Right Bottom Right
			-0.5f, -0.5f, 0.5f,
			0.0f, 1.0f, 0.0f,
			1.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,

			// 10L: Left Top Right
			-0.5f, 0.5f, 0.5f,
			1.0f, 1.0f, 0.0f,
			1.0f, 1.0f,
			-1.0f, 0.0f, 0.0f,

			// 11L: Left Top Left
			-0.5f, 0.5f, -0.5f,
			1.0f, 0.5f, 0.0f,
			0.0f, 1.0f,
			-1.0f, 0.0f, 0.0f,

		// Under Face
			// 12U: Under Bottom Left
			-0.5f, -0.5f, -0.5f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 
			0.0f, -1.0f, 0.0f,

			// 13U: Under Bottom Right
			0.5f, -0.5f, -0.5f,
			0.0f, 1.0f, 0.0f, 
			1.0f, 0.0f,
			0.0f, -1.0f, 0.0f,

			// 14U: Under Top Right
			0.5f, -0.5f, 0.5f,
			0.0f, 0.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, -1.0f, 0.0f,

			// 15U: Under Top Left
			-0.5f, -0.5f, 0.5f,
			0.5f, 0.0f, 1.0f,
			0.0f, 1.0f,
			0.0f, -1.0f, 0.0f,

		// Right Face
			// 16R: Right Bottom Left
			0.5f, -0.5f, 0.5f,
			1.0f, 0.0f, 0.2f,
			0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,

			// 17R: Right Bottom Right
			0.5f, -0.5f, -0.5f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 0.0f, 0.0f,

			// 18R: Right Top Right
			0.5f, 0.5f, -0.5f,
			0.0f, 0.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, 0.0f, 0.0f,

			// 19R: Right Top Left
			0.5f, 0.5f, 0.5f,
			0.0f, 0.0f, 1.0f,
			0.0f, 1.0f, 
			1.0f, 0.0f, 0.0f,
		
		// Top Face
			// 20T: Top Bottom Left
			-0.5f, 0.5f, 0.5f,
			1.0f, 0.0f, 1.0f,
			0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,

			// 21T: Top Bottom Right
			0.5f, 0.5f, 0.5f,
			0.0f, 1.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,

			// 22T: Top Top Right
			0.5f, 0.5f, -0.5f,
			1.0f, 1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f, 0.0f,

			// 23T: Top Top Left
			-0.5f, 0.5f, -0.5f,
			1.0f, 0.0f, 1.0f,
			0.0f, 1.0f,
			0.0f, 1.0f, 0.0f,
	};
}

void RectangularPrism::buildIndexArray1() {
	indices = {
		0, 1, 2, // front left triangle
		2, 3, 0, // front right triangle

		5, 4, 7, // back left triangle
		7, 6, 5, // back right triangle

		12, 13, 14, // bottom left triangle
		14, 15, 12, // bottom right triangle

		16, 17, 18, // right left triangle
		18, 19, 16, // right right triangle

		20, 21, 22, // top left triangle
		22, 23, 20, // top right triangle

		8, 9, 10, // left left triangle
		10, 11, 8, // left right triangle
	};
}






//void RectangularPrism::buildVertexArray() {
//	vertices = new GLfloat[vertexArraySize]{
//		// 0: Front Bottom Left
//		-0.5f, -0.5f, 0.5f, // x, y, z
//		1.0f, 0.0f, 0.0f, // r, g, b
//		0.0f, 0.0f, // s, t
//		1.0f, 1.0f, 1.0f, // Normals nx, ny, nz
//
//		// 1: Bottom Front Right
//		0.5f, -0.5f, 0.5f,
//		0.0f, 1.0f, 0.0f,
//		1.0f, 0.0f,
//		1.0f, 1.0f, 1.0f,
//
//		// 2: Front Top Right
//		0.5f, 0.5f, 0.5f,
//		0.0f, 0.0f, 1.0f,
//		1.0f, 1.0f,
//		1.0f, 1.0f, 1.0f,
//
//		// 3: Front Top Left
//		-0.5f, 0.5f, 0.5f,
//		1.0f, 0.0f, 0.0f,
//		0.0f, 1.0f,
//		1.0f, 1.0f, 1.0f,
//
//		// 4: Back Bottom Left
//		-0.5f, -0.5f, -0.5f,
//		0.0f, 1.0f, 0.0f,
//		0.0f, 0.0f,
//		-1.0f, -1.0f, -1.0f,
//
//		// 5: Back Top Left 
//		0.5f, -0.5f, -0.5f,
//		0.0f, 0.0f, 1.0f,
//		1.0f, 0.0f,
//		-1.0f, -1.0f, -1.0f,
//
//		// 6: Back Top Right 
//		0.5f, 0.5f, -0.5f,
//		1.0f, 1.0f, 0.0f,
//		1.0f, 1.0f,
//		-1.0f, -1.0f, -1.0f,
//
//		// 7: Back Bottom Right
//		-0.5f, 0.5f, -0.5f,
//		0.0f, 1.0f, 1.0f,
//		0.0f, 1.0f,
//		-1.0f, -1.0f, -1.0f
//	};
//}
//
//void RectangularPrism::buildIndexArray() {
//	indices = new unsigned int [indexArraySize] {
//		0, 1, 2, // front left triangle
//		2, 3, 0, // front right triangle
//
//		5, 4, 7, // back left triangle
//		7, 6, 5, // back right triangle
//
//		4, 5, 1, // bottom left triangle
//		1, 0, 4, // bottom right triangle
//
//		1, 5, 6, // right left triangle
//		6, 2, 1, // right right triangle
//
//		3, 2, 6, // top left triangle
//		6, 7, 3, // top right triangle
//
//		4, 0, 3, // left left triangle
//		3, 7, 4, // left right triangle
//	};
//}

