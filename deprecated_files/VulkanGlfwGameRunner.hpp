#pragma once

// we can use vulkan through glfw
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// project libraries
#include "VulkanRendererGlfw.hpp"
#include "GlfwWindow.hpp"

class VulkanGlfwGameRunner {
private:
	VulkanRendererGlfw vk;
	GlfwWindow glfwWindow;

public:
	VulkanGlfwGameRunner() {
		glfwWindow = GlfwWindow(800, 600);
		vk = VulkanRendererGlfw();
	}

	~VulkanGlfwGameRunner() {
		// Clean up and exit
		glfwDestroyWindow(glfwWindow.getWindow());
		glfwTerminate();
	}

	int initializeGame() {
		glfwWindow.initializeVulkan();
		return vk.initialize(&glfwWindow);
	}

	void run() {
		// Simple loop
		while (!glfwWindowShouldClose(glfwWindow.getWindow())) {
			glfwPollEvents();
		}

		vk.cleanUp();
	}

};