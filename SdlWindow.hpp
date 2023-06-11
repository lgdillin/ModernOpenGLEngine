#pragma once

#include <iostream>
#include <SDL.h>
#include <GL/glew.h>

class SdlWindow {
public:
	SDL_Window* window;

	SdlWindow() {
		window = nullptr;
	}

	int create() {
		// Window dimensions
		const GLint WIDTH = 800, HEIGHT = 600;

		// Frame rate
		const int FPS = 60;
		const int FRAME_RATE = 1000 / FPS;

		// Set SDL attributes
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

		// Profile is how OpenGL will treat the code that is passed
		// This throws an error and won't allow backward compatibility
		// Core Profile - no backwards compatibility
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		// SDL_GL_SetAttribute(SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG, GL_TRUE);


		// Attempt to initialize SDL
		if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
			std::cout << "SDL init failed with: "
				<< SDL_GetError() << std::endl;
			return 1;
		}

		// Attempt to create a window
		SDL_Window* window;
		window = SDL_CreateWindow(
			"OpenGL",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			WIDTH,
			HEIGHT,
			SDL_WINDOW_OPENGL
		);

		// If window failed to create
		if (!window) {
			std::cout << "SDL Window init failed" << std::endl;
			return 1;
		}

		// attempt to create a window context
		SDL_GLContext openGLContext = SDL_GL_CreateContext(window);
		if (!openGLContext) {
			std::cout << "SDDL_GL_CreateContext failed: "
				<< SDL_GetError() << std::endl;
			return 1;
		}


		// Initialize GLEW components
		glewExperimental = GL_TRUE;
		GLenum error = glewInit();
		if (error != GLEW_OK) {
			std::cout << "glewInit failed" << std::endl;
			SDL_DestroyWindow(window);
			SDL_GL_DeleteContext(openGLContext);
			return 1;
		}

		// This  makes our buffer swap syncronized with
		// The monitor's vertical refresh
		SDL_GL_SetSwapInterval(1);

		// create the glViewport inside the window
		glViewport(0, 0, WIDTH, HEIGHT);


		// Game Loop
		float lastFrame = SDL_GetTicks();
		bool windowIsOpen = true;
		while (windowIsOpen) {
			float elapsed = SDL_GetTicks() - lastFrame;

			if (elapsed < FRAME_RATE) {
				SDL_Delay(FRAME_RATE - elapsed);
			}

			float deltaTime = (SDL_GetTicks() - lastFrame) / 1000.0f;
			std::cout << "deltaTime: " << deltaTime << std::endl;

			// Process Controller update (ie user input)
			SDL_Event event;
			if (SDL_PollEvent(&event) > 0) {
				if (event.type == SDL_QUIT) windowIsOpen = false;
				if (event.type == SDL_KEYDOWN) windowIsOpen = false;
			}

			// Process simulation model updates

			// Process View updates
			// (Render to the frame)
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			SDL_GL_SwapWindow(window);
		}

		return 0;
	}

private:

};