#pragma once

#include <GL/glew.h>

#include "GlfwCamera.hpp"

class CameraBall {
	CameraBall() {
		radius = 0.5f;
	}

	CameraBall(GlfwCamera* _glfwCamera) {
		glfwCamera = _glfwCamera;
		radius = 0.5f;
	}

	bool collision() {
		GLfloat xPos = glfwCamera->getCameraPosition().x;
		GLfloat yPos = glfwCamera->getCameraPosition().y;
		GLfloat zPos = glfwCamera->getCameraPosition().z;

		return false;
	}

	~CameraBall() {}

private:
	GlfwCamera *glfwCamera;

	GLfloat radius;
};