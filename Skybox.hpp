#pragma once

#include "stb_image.cpp"

#include <vector>
#include <array>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "Mesh.hpp"

class Skybox {
public:
	Skybox();
	Skybox(std::vector<std::string> _faceLocations);
	Skybox(std::string *_faceLocations, int _faces = 6);

	~Skybox();

	void drawSkybox(glm::mat4 _viewMatrix, glm::mat4 _projectionMatrix);

private:
	Mesh *m_skyboxMesh;
	Shader *m_skyboxShader;

	GLuint 
		m_textureId, 
		m_uniformProjection, 
		m_uniformView;

	std::array<GLfloat, 88> m_skyboxVertices;
	std::array<unsigned int, 36> m_skyboxIndices;
	void generateVerticesAndIndices();
};