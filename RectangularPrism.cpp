#include "RectangularPrism.hpp"

RectangularPrism::RectangularPrism() {
	vertexArraySize = 64;
	indexArraySize = 36;
	buildVertexArray();
	buildIndexArray();

	mesh = new Mesh();
	initMesh();

	model = glm::mat4(1);

	rotationAngle = 0.0f;
	translationVector = glm::vec3(1.0f, 1.0f, 1.0f);
	rotationVector = glm::vec3(0.0f, 1.0f, 0.0f);
	scalingVector = glm::vec3(1.0f, 1.0f, 1.0f);
}

RectangularPrism::RectangularPrism(Texture& _texture) {
	vertexArraySize = 64;
	indexArraySize = 36;
	buildVertexArray();
	buildIndexArray();

	mesh = new Mesh();
	initMesh();

	texture = &_texture;
	//texture->loadTexture();

	model = glm::mat4(1);

	rotationAngle = 0.0f;
	translationVector = glm::vec3(1.0f, 1.0f, 1.0f);
	rotationVector = glm::vec3(0.0f, 1.0f, 0.0f);
	scalingVector = glm::vec3(1.0f, 1.0f, 1.0f);
}

RectangularPrism::~RectangularPrism() {
	//delete[] vertices;
	//delete[] indices;
}

void RectangularPrism::transform() {
	model = glm::mat4(1);
	model = glm::translate(model, translationVector);
	model = glm::rotate(model, rotationAngle, rotationVector);
	model = glm::scale(model, scalingVector);
	scaleTexture();
}

glm::mat4 RectangularPrism::scaleTexture() {
	scale = glm::mat4(1);
	//scale = glm::scale(scale, scalingVector);
	return scale;
}

void RectangularPrism::initMesh() {
	mesh->createMesh(vertices, indices, vertexArraySize, indexArraySize);
}

void RectangularPrism::swapColors(glm::vec3 color) {
	// each vertex has 8 components, and we want components 3,4,5
	for (int i = 0; i < vertexArraySize / 8; ++i) {
		vertices[i * 8 + 3] = color.x;
		vertices[i * 8 + 4] = color.y;
		vertices[i * 8 + 5] = color.z;
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

void RectangularPrism::buildVertexArray() {
	vertices = new GLfloat[vertexArraySize]{
		// 0: Front Bottom Left
		-0.5f, -0.5f, 0.5f, // x, y, z
		0.0f, 0.0f, 0.0f, // r, g, b
		0.0f, 0.0f, // s, t

		// 1: Bottom Front Right
		0.5f, -0.5f, 0.5f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f,

		// 2: Front Top Right
		0.5f, 0.5f, 0.5f,
		0.0f, 0.0f, 0.0f,
		1.0f, 1.0f,

		// 3: Front Top Left
		-0.5f, 0.5f, 0.5f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f,

		// 4: Back Bottom Left
		-0.5f, -0.5f, -0.5f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f,

		// 5: Back Top Left 
		0.5f, -0.5f, -0.5f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f,

		// 6: Back Top Right 
		0.5f, 0.5f, -0.5f,
		0.0f, 0.0f, 0.0f,
		1.0f, 1.0f,

		// 7: Back Bottom Right
		-0.5f, 0.5f, -0.5f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
	};
}

void RectangularPrism::buildIndexArray() {
	indices = new unsigned int [indexArraySize] {
		0, 1, 2, // front left triangle
		2, 3, 0, // front right triangle

		5, 4, 7, // back left triangle
		7, 6, 5, // back right triangle

		4, 5, 1, // bottom left triangle
		1, 0, 4, // bottom right triangle

		1, 5, 6, // right left triangle
		6, 2, 1, // right right triangle

		3, 2, 6, // top left triangle
		6, 7, 3, // top right triangle

		4, 0, 3, // left left triangle
		3, 7, 4, // left right triangle
	};
}

