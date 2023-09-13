#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include <array>
#include <set>

#include "GlfwWindow.hpp"
// #define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>


#include "Utilities.hpp"
#include "VulkanValidation.hpp"

//constexpr auto VALIDATION_ENABLED = false;

class VulkanRendererGlfw {
public:
	VulkanRendererGlfw();

	~VulkanRendererGlfw();

	int initialize(GlfwWindow *_glfwWindow);
	void cleanUp();

	void setGlfwWindow(GlfwWindow *_glfwWindow);

private:
	GlfwWindow *m_glfwWindow;

	// Vulkan components
	VkInstance m_instance;
	VkDebugReportCallbackEXT m_callback;

	// Holds physical/logical device
	struct {
		VkPhysicalDevice physicalDevice;
		VkDevice logicalDevice;
	} m_mainDevice;

	VkQueue m_graphicsQueue, m_presentationQueue;

	VkSurfaceKHR m_surface;

	// Vulkan - create functions
	void createInstance();
	void createDebugCallback();
	void createLogicalDevice();
	void createSurface();

	// Vulkan - get functions
	void getPhysicalDevice();

	// Vulkan - Support/validation function
	bool checkInstanceExtensionSupport(
		std::vector<const char *> *checkExtensions);
	bool checkDeviceSuitable(VkPhysicalDevice pDevice);

	// Getter functions
	QueueFamilyIndices getQueueFamilies(VkPhysicalDevice pDevice);
	SwapChainDetails getSwapChainDetails(VkPhysicalDevice pDevice);
};