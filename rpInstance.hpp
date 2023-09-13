#pragma once

#include <iostream>

#include <GL/glew.h>

#include <glm/glm.hpp>

#include "Texture.hpp"

struct RpInstance {
	glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 m_color = glm::vec3(1.0f, 1.0f, 1.0f);

	float m_rotationAngleRadians = 0.0f;


	std::string m_description;
	Texture *m_diffuseTexture = nullptr;
	Texture *m_specularTexture = nullptr;;
};