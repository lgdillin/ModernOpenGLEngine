#pragma once

#include "Texture.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Sprite : public Texture{
public:
	Sprite();
	Sprite(
		glm::vec3 _position,
		std::string _fileLocation,
		glm::vec3 _color = glm::vec3(1.0f)
	);

	~Sprite();

	void initialize();

	void render();

private:
	glm::vec3 m_position, m_color;
	glm::mat4 m_projection;

	GLuint m_vertexArrayObject;
};