#pragma once

#include "VulkanHeaderLoadOrder.hpp"

#include <iostream>
#include <cmath>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.hpp"
#include "Shader.hpp"

class GlfwWindow {
private:

	bool firstMouseMovement;
	GLint
		windowWidth, windowHeight,
		bufferWidth, bufferHeight;

	GLfloat
		lastMouseXPosition, lastMouseYPosition,
		mouseDeltaX, mouseDeltaY;


	glm::mat4 projection;

	GLFWwindow* window;

	bool keys[1024]; // covers range of ASCII characters for keypresses
	bool keyHit[1024]; // adds an extra binary value for toggle-able keys

	static void keyHandlerCallback(
		GLFWwindow* window,
		int key, // the actual value of the key (0 <= key <= 1024)
		int code,
		int action, // action ie. press/release of the key
		int mode
	) {
		GlfwWindow* _window = static_cast<GlfwWindow*>(
			glfwGetWindowUserPointer(window));

		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		if (key >= 0 && key < 1024) {
			if (action == GLFW_PRESS) {
				_window->keys[key] = true;
			} else if (action == GLFW_RELEASE) {
				_window->keys[key] = false;
			}
		}
	}

	static void mouseHandlerCallback(GLFWwindow* window, 
		double xPosition, double yPosition) 
	{
		GlfwWindow* _window = static_cast<GlfwWindow*>(
			glfwGetWindowUserPointer(window));

		if (_window->firstMouseMovement) {
			_window->lastMouseXPosition = xPosition;
			_window->lastMouseYPosition = yPosition;
			_window->firstMouseMovement = false;
		}

		_window->mouseDeltaX = xPosition - _window->lastMouseXPosition;
		_window->mouseDeltaY = _window->lastMouseYPosition - yPosition;

		_window->lastMouseXPosition = xPosition;
		_window->lastMouseYPosition = yPosition;
	}

public:
	GlfwWindow(GLint wWidth=800, GLint wHeight=600) {
		windowWidth = wWidth;
		windowHeight = wHeight;
		bufferWidth = bufferHeight = 0;
		lastMouseXPosition = lastMouseYPosition = 0;
		mouseDeltaX = mouseDeltaY = 0;

		projection = glm::mat4();
		window = NULL;

		firstMouseMovement = true;

		// fill the array of bools
		for (size_t i = 0; i < 1024; ++i) {
			keys[i] = false;
			keyHit[i] = false;
		}
	}

	~GlfwWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	GLFWwindow *getWindow() { return window; }
	glm::mat4 getProjection() { return projection; }
	GLint getBufferWidth() { return bufferWidth; }
	GLint getBufferHeight() { return bufferHeight; }
	GLint getWindowWidth() { return windowWidth; }
	GLint getWindowHeight() { return windowHeight; }

	bool getShouldClose() { return glfwWindowShouldClose(window); }

	void swapBuffers() { glfwSwapBuffers(window); }

	bool *getKeys() { return keys; }
	bool *getKeyHit() { return keyHit; }

	GLfloat getMouseDeltaX() {
		GLfloat deltaX = mouseDeltaX;
		mouseDeltaX = 0.0f;
		return deltaX;
	}

	GLfloat getMouseDeltaY() {
		GLfloat deltaY = mouseDeltaY;
		mouseDeltaY = 0.0f;
		return deltaY;
	}

	void resetViewport() {
		glViewport(0, 0, bufferWidth, bufferHeight);
	}

	void initializeVulkan() {
		if (!glfwInit()) {
			printf("Error Initialising GLFW");
			glfwTerminate();
			return;
		}

		// GLFW_NO_API designates Vulkan as the API
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(
			windowWidth,
			windowHeight,
			"Vulkan Window",
			nullptr, // use this if you want to make it fullscreen
			nullptr // an extra window to share resources with
		);

		uint32_t count = 0;
		vkEnumerateInstanceExtensionProperties(
			nullptr,
			&count,
			nullptr
		);

		std::cout << "Extension count: " << count << std::endl;


	}


	// Initialize our GLFW Window
	int initialize() {
		if (!glfwInit())
		{
			printf("Error Initialising GLFW");
			glfwTerminate();
			return 1;
		}
		glewExperimental = GL_TRUE;

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

		GLenum error = glGetError();
		if (!window) {
			std::cout << "GLFW window creation failed with err: " << error << std::endl;
			glfwTerminate();
			throw;
		}

		// Size of the main frame buffer we will be using
		glfwGetFramebufferSize(
			window, &bufferWidth, &bufferHeight);

		// Handle Key/Mouse input
		glfwSetKeyCallback(window, keyHandlerCallback);
		glfwSetCursorPosCallback(window, mouseHandlerCallback);
		// Lock the cursor in the center of the screen and hide it
		glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

		glfwSetWindowUserPointer(window, this);

		projection = glm::perspective(
			glm::radians(60.0f),
			(GLfloat)(bufferWidth / bufferHeight),
			0.1f,
			100.0f);

	}
};