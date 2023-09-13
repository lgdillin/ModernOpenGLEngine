#include "RectangularPrismBuilder.hpp"

std::array <GLfloat, RectangularPrismBuilder::m_boundingBoxSize> 
	RectangularPrismBuilder::m_boundingBoxSource 
	= std::array<GLfloat, RectangularPrismBuilder::m_boundingBoxSize>();

std::array <GLfloat, RectangularPrismBuilder::m_boundingBoxSize> 
	RectangularPrismBuilder::m_boundingBoxTransformed 
	= std::array<GLfloat, RectangularPrismBuilder::m_boundingBoxSize>();

void RectangularPrismBuilder::initialize() {
	computeInterpolatedNormalsSmoothEdges();
	initializeSourceBoundingBox();
}

RectPrism RectangularPrismBuilder::build(RpInstance _rpInstance) {
	RectPrism rp = RectPrism();

	Mesh *mesh = new Mesh();
	mesh->createMesh(
		RectangularPrismBuilder::m_vertices,
		RectangularPrismBuilder::m_indices,
		RectangularPrismBuilder::m_vertexDataSize,
		RectangularPrismBuilder::m_indicesDataSize);
	rp.setMesh(mesh);

	// init data
	rp.setDescription(_rpInstance.m_description);
	rp.setDiffuseTexture(_rpInstance.m_diffuseTexture);
	rp.setSpecularTexture(_rpInstance.m_specularTexture);

	// set up geometrical data
	rp.setPosition(_rpInstance.m_position);
	rp.setRotationalAxis(_rpInstance.m_rotationAxis);
	rp.setScale(_rpInstance.m_scale);
	rp.setColor(_rpInstance.m_color);
	rp.setRotationalAngle(_rpInstance.m_rotationAngleRadians);

	rp.setBoundingBoxPointer(m_boundingBoxSource);

	return rp;
}

void RectangularPrismBuilder::initializeSourceBoundingBox() {
	GLfloat xMin = RectangularPrismBuilder::m_vertices[0];
	GLfloat xMax = RectangularPrismBuilder::m_vertices[0];

	GLfloat yMin = RectangularPrismBuilder::m_vertices[1];
	GLfloat yMax = RectangularPrismBuilder::m_vertices[1];

	GLfloat zMin = RectangularPrismBuilder::m_vertices[2];
	GLfloat zMax = RectangularPrismBuilder::m_vertices[2];
	for (int i = 0; i < RectangularPrismBuilder::m_vertexDataSize / RectangularPrismBuilder::m_numberOfVertexComponents; ++i) {
		if (RectangularPrismBuilder::m_vertices[RectangularPrismBuilder::m_numberOfVertexComponents * i] < xMin) {
			xMin = RectangularPrismBuilder::m_vertices[RectangularPrismBuilder::m_numberOfVertexComponents * i];
		}

		if (RectangularPrismBuilder::m_vertices[RectangularPrismBuilder::m_numberOfVertexComponents * i] > xMax) {
			xMax = RectangularPrismBuilder::m_vertices[RectangularPrismBuilder::m_numberOfVertexComponents * i];
		}

		if (RectangularPrismBuilder::m_vertices[RectangularPrismBuilder::m_numberOfVertexComponents * i + 1] < yMin) {
			yMin = RectangularPrismBuilder::m_vertices[RectangularPrismBuilder::m_numberOfVertexComponents * i + 1];
		}

		if (RectangularPrismBuilder::m_vertices[RectangularPrismBuilder::m_numberOfVertexComponents * i + 1] > yMax) {
			yMax = RectangularPrismBuilder::m_vertices[RectangularPrismBuilder::m_numberOfVertexComponents * i + 1];
		}

		if (RectangularPrismBuilder::m_vertices[RectangularPrismBuilder::m_numberOfVertexComponents * i + 2] < zMin) {
			zMin = RectangularPrismBuilder::m_vertices[RectangularPrismBuilder::m_numberOfVertexComponents * i + 2];
		}

		if (RectangularPrismBuilder::m_vertices[RectangularPrismBuilder::m_numberOfVertexComponents * i + 2] > zMax) {
			zMax = RectangularPrismBuilder::m_vertices[RectangularPrismBuilder::m_numberOfVertexComponents * i + 2];
		}
	}

	// vertices are defined front to back, CCW starting bottom left
	// vertex 0:
	RectangularPrismBuilder::m_boundingBoxSource[0] = xMin;
	RectangularPrismBuilder::m_boundingBoxSource[1] = yMin;
	RectangularPrismBuilder::m_boundingBoxSource[2] = zMax;

	// vertex 1;:
	RectangularPrismBuilder::m_boundingBoxSource[3] = xMax;
	RectangularPrismBuilder::m_boundingBoxSource[4] = yMin;
	RectangularPrismBuilder::m_boundingBoxSource[5] = zMax;

	//vertex 2
	RectangularPrismBuilder::m_boundingBoxSource[6] = xMax;
	RectangularPrismBuilder::m_boundingBoxSource[7] = yMax;
	RectangularPrismBuilder::m_boundingBoxSource[8] = zMax;

	// vertex 3
	RectangularPrismBuilder::m_boundingBoxSource[9] = xMin;
	RectangularPrismBuilder::m_boundingBoxSource[10] = yMax;
	RectangularPrismBuilder::m_boundingBoxSource[11] = zMax;

	// vertex 4
	RectangularPrismBuilder::m_boundingBoxSource[12] = xMin;
	RectangularPrismBuilder::m_boundingBoxSource[13] = yMin;
	RectangularPrismBuilder::m_boundingBoxSource[14] = zMin;

	// vertex 5
	RectangularPrismBuilder::m_boundingBoxSource[15] = xMax;
	RectangularPrismBuilder::m_boundingBoxSource[16] = yMin;
	RectangularPrismBuilder::m_boundingBoxSource[17] = zMin;

	// vertex 6
	RectangularPrismBuilder::m_boundingBoxSource[18] = xMax;
	RectangularPrismBuilder::m_boundingBoxSource[19] = yMax;
	RectangularPrismBuilder::m_boundingBoxSource[20] = zMin;

	// vertex 7
	RectangularPrismBuilder::m_boundingBoxSource[21] = xMin;
	RectangularPrismBuilder::m_boundingBoxSource[22] = yMax;
	RectangularPrismBuilder::m_boundingBoxSource[23] = zMin;
}

void RectangularPrismBuilder::computeInterpolatedNormalsSmoothEdges() {
	unsigned int index0, index1, index2;
	for (size_t i = 0; i < RectangularPrismBuilder::m_numberOfVertexComponents / 3; ++i) {

		// Cycle through every triangle defined in our Index Array
		// Store the index data in index0, index1, and index2
		// The purpose of this code is to grab the actual vertex values that
		// exist at each one of these indices, and use the value of the index
		// to map to adjacent vertices in the vertex data
		index0 = RectangularPrismBuilder::m_indices[i * 3] 
			* RectangularPrismBuilder::m_numberOfVertexComponents;
		index1 = RectangularPrismBuilder::m_indices[i * 3 + 1] 
			* RectangularPrismBuilder::m_numberOfVertexComponents;
		index2 = RectangularPrismBuilder::m_indices[i * 3 + 2] 
			* RectangularPrismBuilder::m_numberOfVertexComponents;


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
			RectangularPrismBuilder::m_vertices[index1] - RectangularPrismBuilder::m_vertices[index0],
			RectangularPrismBuilder::m_vertices[index1 + 1] - RectangularPrismBuilder::m_vertices[index0 + 2],
			RectangularPrismBuilder::m_vertices[index1 + 2] - RectangularPrismBuilder::m_vertices[index0 + 2]);

		glm::vec3 v2(
			RectangularPrismBuilder::m_vertices[index2] - RectangularPrismBuilder::m_vertices[index0],
			RectangularPrismBuilder::m_vertices[index2 + 1] - RectangularPrismBuilder::m_vertices[index0 + 2],
			RectangularPrismBuilder::m_vertices[index2 + 2] - RectangularPrismBuilder::m_vertices[index0 + 2]);

		// Now that both vectors V_index1 and V_index2 no longer have 
		// any shared V_index data. Then we produce the cross product,
		// giving us a vector perpindicular to the two vectors, 
		// and normalize it.
		glm::vec3 normal = glm::normalize(glm::cross(v1, v2));

		// Add the index offset for where the normal data is stored
		// in each vertex within the vertices array
		index0 += RectangularPrismBuilder::m_offsetForVertexNormals;
		index1 += RectangularPrismBuilder::m_offsetForVertexNormals;
		index2 += RectangularPrismBuilder::m_offsetForVertexNormals;

		// Copy the data over, writing our interpolated normalized normals
		// To their relevant location in the vertex data
		RectangularPrismBuilder::m_vertices[index0] += normal.x;
		RectangularPrismBuilder::m_vertices[index0 + 1] += normal.y;
		RectangularPrismBuilder::m_vertices[index0 + 2] += normal.z;

		RectangularPrismBuilder::m_vertices[index1] += normal.x;
		RectangularPrismBuilder::m_vertices[index1 + 1] += normal.y;
		RectangularPrismBuilder::m_vertices[index1 + 2] += normal.z;

		RectangularPrismBuilder::m_vertices[index2] += normal.x;
		RectangularPrismBuilder::m_vertices[index2 + 1] += normal.y;
		RectangularPrismBuilder::m_vertices[index2 + 2] += normal.z;
	}

	unsigned int nOffset;
	for (size_t i = 0; i < RectangularPrismBuilder::m_vertexDataSize / RectangularPrismBuilder::m_numberOfVertexComponents; ++i) {

		// Copy a vector from out Vertex Data and normalize it
		nOffset = i * RectangularPrismBuilder::m_numberOfVertexComponents 
			+ RectangularPrismBuilder::m_offsetForVertexNormals;
		glm::vec3 offsetVector = glm::normalize(glm::vec3(
			RectangularPrismBuilder::m_vertices[nOffset],
			RectangularPrismBuilder::m_vertices[nOffset + 1],
			RectangularPrismBuilder::m_vertices[nOffset + 2]));

		// Replace the vector values inside the Vertex Data with
		// our new normalized values
		RectangularPrismBuilder::m_vertices[nOffset] = offsetVector.x;
		RectangularPrismBuilder::m_vertices[nOffset + 1] = offsetVector.y;
		RectangularPrismBuilder::m_vertices[nOffset + 2] = offsetVector.z;

	}
}

unsigned int RectangularPrismBuilder::m_indices[RectangularPrismBuilder::m_indicesDataSize] = {
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

GLfloat RectangularPrismBuilder::m_vertices[RectangularPrismBuilder::m_vertexDataSize] = {
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