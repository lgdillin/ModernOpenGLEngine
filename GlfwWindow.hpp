#pragma once

#include <iostream>
#include <cmath>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.hpp"
#include "Shader.hpp"

class GlfwWindow {
private:
	GLint
		windowWidth, windowHeight,
		bufferWidth, bufferHeight;

	glm::mat4 projection;

	GLFWwindow* window;

public:
	GlfwWindow(GLint wWidth=800, GLint wHeight=600) {
		windowWidth = wWidth;
		windowHeight = wHeight;
		bufferWidth = 0;
		bufferHeight = 0;

		projection = glm::mat4();
		window = 0;

	}

	~GlfwWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	glm::mat4 getProjection() { return projection; }
	GLint getBufferWidth() { return bufferWidth; }
	GLint getBufferHeight() { return bufferHeight; }

	bool getShouldClose() { return glfwWindowShouldClose(window); }

	void swapBuffers() { glfwSwapBuffers(window); }


	// Initialize our GLFW Window
	int initialize() {


		// Setup GLFW window properties
		// OpenGL version
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

		// Core profile - no backwards compatibility
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// Allow forward compatibility
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

		// create a window
		window = glfwCreateWindow(
			windowWidth,
			windowHeight,
			"GLFW window",
			NULL,
			NULL
		);

		if (!window) {
			std::cout << "GLFW window creation failed" << std::endl;
			glfwTerminate();
			return 1;
		}

		// Size of the main frame buffer we will be using
		glfwGetFramebufferSize(
			window, &bufferWidth, &bufferHeight);

		// Set the Context for GLEW to use
		// if we have multiple windows, we can switch the context
		// For which window we want to draw in
		glfwMakeContextCurrent(window);

		// Allow modern extension features
		// Enable us to access extensions 
		// for advanced development in the future
		glewExperimental = GL_TRUE;

		if (glewInit() != GLEW_OK) {
			std::cout << "glewInit() failed" << std::endl;
			glfwDestroyWindow(window);
			glfwTerminate();
			return 1;
		}

		// Enable depth testing so we are making sure we are drawing
		// in the correct order
		glEnable(GL_DEPTH_TEST);

		// Set up Viewport size (opengl not glfw)
		glViewport(0, 0, bufferWidth, bufferHeight);

		glm::perspective(
			45.0f,
			(GLfloat)(bufferWidth / bufferHeight),
			0.1f,
			100.0f);

	}
};