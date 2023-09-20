#pragma once

#include "VulkanHeaderLoadOrder.hpp"
#include "VulkanGlfwDeviceBuilder.hpp"
#include "VulkanLogging.hpp"

#include "GlfwWindow.hpp"

class VulkanGlfwEngine {
public:
	VulkanGlfwEngine();
	~VulkanGlfwEngine();

private:
	bool m_debugMode = true;
	const char *testExtensions = "VK_EXT_debug_utils";
	const char *testExtensions2 = "VK_KHR_surface";
	//const char *testLayers = "VK_LAYER_KHRONOS_validation";

	// window params
	int m_width = 800;
	int m_height = 600;
	GlfwWindow m_window = NULL;
	//GLFWwindow *m_window;
	

	vk::Instance m_instance = nullptr;
	vk::PhysicalDevice m_physicalDevice = nullptr;
	vkGlfw::QueueFamilyIndices m_queueFamilyIndices;
	vk::Device m_logicalDevice = nullptr;
	vk::Queue m_presentQueue = nullptr;
	vk::Queue m_graphicsQueue = nullptr;
	vk::SurfaceKHR m_surface;
	vkGlfw::SwapchainSupportDetails m_supportDetails;
	
	// swapchain 
	vk::SwapchainKHR m_swapchain;
	std::vector<vk::Image> m_swapchainImages;
	vk::Format m_swapchainFormat;
	vk::Extent2D m_swapchainExtent;

	static vk::DebugUtilsMessengerEXT m_debugMessenger;
	static vk::DispatchLoaderDynamic m_dispatcher;

	///////////////////////////////////////////////
	//void presentQue

	///////////////////////////////////////////////
	// GLFW setup
	void buildWindow();
	void buildInstance();
	void buildWindowSurface();
	void buildDebugMessenger();

	void querySwapchainSupport();

	bool checkExtensionAndLayerSupport(
		std::vector<const char *> &_extensions,
		std::vector<const char *> &_layers
	);

	//void findQueueFamilies();

	// declared and implemented
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
		void *pUserData
	) {
		std::cerr << "validation layer: " << pCallbackData->pMessage << "\n";
		return VK_FALSE;
	}
};