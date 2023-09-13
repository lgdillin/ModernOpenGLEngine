#pragma once

#include <iostream>
#include <array>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Mesh.hpp"
#include "Texture.hpp"
#include "Shader.hpp"

class RectPrism {
public:
	static const unsigned int m_boundingBoxSize = 24;

	RectPrism();
	~RectPrism();

	void setPosition(glm::vec3 _position);
	void setRotationalAxis(glm::vec3 _axis);
	void setScale(glm::vec3 _scale);
	void setColor(glm::vec3 _color);
	void setRotationalAngle(float _angle);

	void setDescription(std::string _description);
	void setMesh(Mesh *_mesh);
	void setDiffuseTexture(Texture *_texture);
	void setSpecularTexture(Texture *_texture);

	void transform(glm::mat4 &_model);
	void draw(Shader &_shader);

	void setBoundingBoxPointer(std::array<GLfloat, RectPrism::m_boundingBoxSize> &_array);
	void transformBoundingBox(glm::mat4 _model);
	void updateBoundingBox();
private:
	std::string m_description;

	glm::vec3 m_position;
	glm::vec3 m_rotationalAxis;
	glm::vec3 m_scale;
	glm::vec3 m_color;
	
	float m_rotationalAngle;

	Mesh *m_mesh;
	Texture *m_diffuseTexture;
	Texture *m_specularTexture;

	std::array<GLfloat, m_boundingBoxSize> m_boundingBoxSource;
	std::array<GLfloat, m_boundingBoxSize> m_boundingBoxTransformed;
};