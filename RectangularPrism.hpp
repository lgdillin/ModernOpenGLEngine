#pragma once

#include <iostream>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.hpp"
#include "Texture.hpp"

// Class to generate an object that starts as a cube, but can be 
// modified to fit 
class RectangularPrism {
public:
	RectangularPrism();
	RectangularPrism(Texture& _texture);
	~RectangularPrism();


	void transform();
	void update();

	GLfloat* getVertexArrayPointer() { return vertices; }
	unsigned int* getIndexArrayPointer() { return indices; }

	GLuint getVertexArraySize() { return vertexArraySize; }
	GLuint getIndexArraySize() { return indexArraySize; }

	int getVertexMemorySize() { return vertexArraySize * sizeof(GLfloat); }
	int getIndexMemorySize() { return indexArraySize * sizeof(int); }

	Mesh* getMesh() { return mesh; }
	void renderMesh() { mesh->renderMesh(); }

	Texture* getTexture() { return texture; }
	void setTexture(Texture &_texture) { texture = &_texture; }

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

	void buildVertexArray();
	void buildIndexArray();

	void pickColor(
		std::string color = "custom",
		glm::vec3 customColor = glm::vec3(1.0f, 1.0f, 1.0f));

	void swapColors(glm::vec3 color);

private:
	GLfloat* vertices;
	unsigned int* indices;

	GLuint vertexArraySize,
		indexArraySize;

	Mesh* mesh;

	Texture* texture;

	glm::mat4 model, scale;

	float rotationAngle;
	glm::vec3
		translationVector,
		rotationVector,
		scalingVector;

	void initMesh();
};