//#define STBI_NO_SIMD
#define STB_IMAGE_IMPLEMENTATION

// Graphics Libraries
#include "VulkanHeaderLoadOrder.hpp"
//#include <SDL.h>

// Developed libraries
//#include "SdlWindow.hpp"
#include "GlfwWindow.hpp"
#include "GlfwGame.hpp"

#include "VulkanApp.hpp"


enum Renderer {
	OPENGL_GLFW,
	OPENGL_SDL,
	VULKAN_GLFW,
	VULKAN_SDL
};

int main(int argc, char* argv[]) {
	/*SdlWindow sdlWindow;
	sdlWindow.create();*/

	Renderer renderer = VULKAN_GLFW;

	if (renderer == OPENGL_GLFW) {
		GlfwGame glfwGame;
		glfwGame.init();
		glfwGame.runGame();
	} else if (renderer == VULKAN_GLFW) {
		VulkanApp app = VulkanApp();
		app.run();
	}

	
	
	return 0;
}