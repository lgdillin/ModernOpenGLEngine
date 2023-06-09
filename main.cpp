#include <iostream>

/*
	Using Glew (Graphics Library Extension Wrangler):
	#include <GL/glew.h>

	after initialising the OpenGL Context:
	glewExperimental = GL_TRUE;
	glewInit();
	Should return GLEW_OK. if it fails, returns an error
	You can read the error with glewGetErrorString(result);

	GLEW can check if extensions exist:
	if(!GLEW_EXT_framebuffer_object) {}

	wglew.h for Windows-specific functions
*/

/*
	GLFW: OpenGL FrameWork 
	- Handles window creation and control
	- Pick up and process input from the keyboard, mouse, joystick, and gamepad
	- Even allows multiple monitor support
	- Uses OpenGL context for windows

*/

/*
	SDL: Simple DirectMedia Layer
	- Can do almost everything GLFW can do
	- Also (Audio, Threading, Filesystems, etc)
	- Very popular, especially for indie devs
	- Used in: FTL, Amnesia, Starbound, and Dying Light
	- Even used in level editors for Source Engine and Cryengine
*/

/*
	SFML: Simple and Fast Multimedia Library
	- Like SDL but with even more features
	- The OpenGL context is very weak. Based on 2D only graphics

	GLUT: OpenGL Utility Toolkit
	- No longer maintained. Avoid using

	Win32 API
	- Lowest level for window creation (for MS Windows)
	- Only attempt if you know what you're doing
*/

int main() {
	return 0;
}