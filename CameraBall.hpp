#pragma once

#include <GL/glew.h>

#include "GlfwCamera.hpp"

#define APPLY_GRAVITY true

class CameraBall {
public:
	CameraBall() {}

	CameraBall(GlfwCamera* _glfwCamera) {
		glfwCamera = _glfwCamera;
		width = 0.2f;
		eyeHeight = 2.0f;
		bodyHeight = 0.05f;
		toggleGravity = APPLY_GRAVITY;

		glfwCamera->setToggleGravity(toggleGravity);
	}

	void applyGravity() {
		if (glfwCamera->getToggleGravity()) {
			glfwCamera->bumpCamera(glm::vec3(0.0f, -0.01f, 0.0f));
		}
	}

	bool getToggleGravity() { return toggleGravity; }

	void setToggleGravity(bool g) {
		toggleGravity = g;
	}

	void jump() {

	}

	GlfwCamera *getCamera() { return glfwCamera; }

	GLfloat getWidth() { return width; }
	GLfloat getEyeHeight() { return eyeHeight; }
	GLfloat getBodyHeight() { return bodyHeight; }

	~CameraBall() {}

private:
	bool toggleGravity;
	GlfwCamera *glfwCamera;

	GLfloat width;
	GLfloat eyeHeight;
	GLfloat bodyHeight;
};