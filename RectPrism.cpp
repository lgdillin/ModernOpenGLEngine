#include "RectPrism.hpp"

RectPrism::RectPrism() {
}

RectPrism::~RectPrism() {
}

void RectPrism::setPosition(glm::vec3 _position) {
	m_position = _position;
}

void RectPrism::setRotationalAxis(glm::vec3 _axis) {
	m_rotationalAxis = _axis;
}

void RectPrism::setScale(glm::vec3 _scale) {
	m_scale = _scale;
}

void RectPrism::setColor(glm::vec3 _color) {
	m_color = _color;
}

void RectPrism::setRotationalAngle(float _angle) {
	m_rotationalAngle = _angle;
}

void RectPrism::setDescription(std::string _description) {
	m_description = _description;
}

void RectPrism::setMesh(Mesh *_mesh) {
	m_mesh = _mesh;
}

void RectPrism::setDiffuseTexture(Texture *_texture) {
	m_diffuseTexture = _texture;
}

void RectPrism::setSpecularTexture(Texture *_texture) {
	m_specularTexture = _texture;
}

void RectPrism::transform(glm::mat4 &_model) {
	_model = glm::translate(_model, m_position);
	_model = glm::rotate(_model, m_rotationalAngle, m_rotationalAxis);
	_model = glm::scale(_model, m_scale);
}

void RectPrism::draw(Shader &_shader) {
	// assign active texture
	unsigned int textureUnitOffset = 1;

	// assign texture type (currently hardcoded)
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(_shader.getM_shaderId(),
		"texture_diffuse1"), 0);
	glBindTexture(GL_TEXTURE_2D, m_diffuseTexture->getTextureId());

	glActiveTexture(GL_TEXTURE1);
	glUniform1i(glGetUniformLocation(_shader.getM_shaderId(),
		"texture_specular1"), 1);
	glBindTexture(GL_TEXTURE_2D, m_specularTexture->getTextureId());

	m_mesh->renderMesh();

	glActiveTexture(GL_TEXTURE0);
}

void RectPrism::setBoundingBoxPointer(
	std::array<GLfloat, RectPrism::m_boundingBoxSize> &_array
) {
	m_boundingBoxSource = _array;
}

void RectPrism::transformBoundingBox(glm::mat4 _model) {
	for (int i = 0; i < 8; ++i) {
		glm::vec4 vector = glm::vec4(
			m_boundingBoxSource[i * 3],
			m_boundingBoxSource[i * 3 + 1],
			m_boundingBoxSource[i * 3 + 2], 1.0f
		);

		//glm::vec4 vector = glm::vec4(
		//	m_boundingBoxSource->at(i * 3),
		//	m_boundingBoxSource->at(i * 3 + 1),
		//	m_boundingBoxSource->at(i * 3 + 2), 1.0f
		//);

		//glm::vec4 transformedVec = _model * vector;
		glm::vec4 transformedVec = glm::vec4(1.0f);

		m_boundingBoxTransformed[i * 3] = transformedVec.x;
		m_boundingBoxTransformed[i * 3 + 1] = transformedVec.y;
		m_boundingBoxTransformed[i * 3 + 2] = transformedVec.z;
	}

	// update bounding box

}

void RectPrism::updateBoundingBox() {

}

