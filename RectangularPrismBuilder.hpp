#pragma once

#include <array>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.hpp"

#include "RectPrism.hpp"

class RectangularPrismBuilder {
public:

	static void initialize();

	static RectPrism build();

	static const unsigned int m_vertexDataSize =
		4 // number of vertices per face
		* 6 // number of faces on a rectangular prism
		* 11; // number of components per vertex

	static const unsigned int m_indicesDataSize =
		3 // incides per triangle
		* 2 // triangles per face
		* 6; // faces per rectangular prism;

	static const unsigned int m_numberOfVertexComponents = 11;
	static const unsigned int m_offsetForVertexNormals = 8;
	static const unsigned int m_boundingBoxSize = 24;

	static GLfloat m_vertices[m_vertexDataSize];
	static unsigned int m_indices[m_indicesDataSize];

	static std::array <GLfloat, m_boundingBoxSize> m_boundingBoxSource;
	static std::array <GLfloat, m_boundingBoxSize> m_boundingBoxTransformed;
private:
	
};