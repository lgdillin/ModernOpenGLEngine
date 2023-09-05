#pragma once

#include <iostream>
#include <array>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.hpp"
#include "Texture.hpp"
#include "Shader.hpp"

// Class to generate an object that starts as a cube, but can be 
// modified to fit 
class RectangularPrism {
public:
	static const unsigned int
		vertexDataLength = (
			4 // Number of vertices per face
			* 6 // number of faces on a rectangular prism
			* 11 // number of components per vertex
		),
		indicesDataLength = (
			3 // indices per triangle
			* 2 // Triangles per face
			* 6 // faces per rectangular prism
		),
		numberOfVertexComponents = 11, // The number of components per vertex
		offsetForVertexNormals = 8; // Where the vertex data begins per vertex

	RectangularPrism();
	~RectangularPrism();
	void initialize(std::string pickColor = "white");

	GLfloat get(size_t index) { return vertices[index]; }

	void transform();
	void update();

	std::array<GLfloat, vertexDataLength> getVertexArray() { return vertices; }
	std::array<unsigned int, indicesDataLength> getIndexArray() { return indices; }

	GLuint getVertexArraySize() { return vertexArraySize; }
	GLuint getIndexArraySize() { return indexArraySize; }

	int getVertexMemorySize() { return vertexArraySize * sizeof(GLfloat); }
	int getIndexMemorySize() { return indexArraySize * sizeof(int); }

	Mesh* getMesh() { return mesh; }
	void renderMesh() { mesh->renderMesh(); }
	void draw(Shader &_shader);

	Texture* getTexture() { return texture; }
	void setTexture(Texture *_texture) { 
		//delete texture;
		texture = _texture; 
	}
	void setSpecularMap(Texture *_map) {
		m_specularMap = _map;
	}

	glm::vec3 getBBVertex(int index);

	void addTextureFromFile(std::string textureFileString);

	glm::vec3 getTranslationVector() { return translationVector; }
	void setTranslationVector(glm::vec3 t) { translationVector = t; }

	float getRotationAngle() { return rotationAngle; }
	void setRotationAngle(float r) { rotationAngle = r; }

	glm::vec3 getRotationVector() { return rotationVector; }
	void setRotationVector(glm::vec3 r) { rotationVector = r; }

	glm::vec3 getScalingVector() { return scalingVector; }
	void setScalingVector(glm::vec3 s) { scalingVector = s; }

	glm::mat4 getModelMatrix() { return model; }
	glm::mat4 scaleTexture();

	void printVertexData();
	void printIndexData();

	void buildVertexArray1();
	void buildVertexArray();
	void buildIndexArray1();
	void buildIndexArray();

	void transformBoundingBox(glm::mat4 model);
	void initBoundingBox();
	void updateBounds();
	GLfloat *getBoundingBox() { return boundingBox; }

	void computeInterpolatedNormalsSmoothEdges();

	void pickColor(
		std::string color = "custom",
		glm::vec3 customColor = glm::vec3(1.0f, 1.0f, 1.0f));

	void swapColors(glm::vec3 color);

	GLfloat getXMinBound() { return xMinBound; }
	GLfloat getXMaxBound() { return xMaxBound; }

	GLfloat getYMinBound() { return yMinBound; }
	GLfloat getYMaxBound() { return yMaxBound; }

	GLfloat getZMinBound() { return zMinBound; }
	GLfloat getZMaxBound() { return zMaxBound; }
private:
	GLfloat 
		immutableBoundingBox[24], // holds the original bounding box
		boundingBox[24]; // copy that is transformed every frame
	GLfloat 
		xMinBound, xMaxBound, 
		yMinBound, yMaxBound, 
		zMinBound, zMaxBound;

	//GLfloat vertices[vertexDataLength];
	//unsigned int indices[indicesDataLength];

	std::array<GLfloat, vertexDataLength> vertices;
	std::array<unsigned int, indicesDataLength> indices;

	GLuint 
		vertexArraySize,
		indexArraySize;




	Mesh* mesh;

	Texture* texture;
	Texture *m_specularMap;

	glm::mat4 model, scale;

	float rotationAngle;
	glm::vec3
		translationVector,
		rotationVector,
		scalingVector;

	void initMesh();
};