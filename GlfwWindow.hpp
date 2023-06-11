#pragma once

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class GlfwWindow {
public:
	GLFWwindow* window;

	bool create() {
		// Window dimensions
		const GLint WIDTH = 800, HEIGHT = 600;

		// Initialize GLFW
		if (!glfwInit()) {
			std::cout << "GLFW init failed" << std::endl;
			glfwTerminate();
			return 1;
		}

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
			WIDTH,
			HEIGHT,
			"Test Window",
			NULL,
			NULL
		);

		if (!window) {
			std::cout << "GLFW window creation failed" << std::endl;
			glfwTerminate();
			return 1;
		}

		// Get Buffer size information
		int bufferWidth, bufferHeight;
		
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

		// Set up Viewport size (opengl not glfw)
		glViewport(0, 0, bufferWidth, bufferHeight);

		// Make our main loop (until window close)
		while (!glfwWindowShouldClose(window)) {
			// Get and handle user input events
			glfwPollEvents();

			// Clear Window
			// Used to wipe the frame to a "blank canvas"
			// Otherwise we could end up with a lot of artifacts
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			// Specify we want to clear the color data in the buffer
			// we can specify we want to clear the depth data
			glClear(GL_COLOR_BUFFER_BIT);

			// OpenGL has two buffers/frames
			// One the user sees, and the one that is being drawn
			// We see the next frame by switching the address the pointer
			// Is pointing to
			glfwSwapBuffers(window);
		}
	}


private:
};