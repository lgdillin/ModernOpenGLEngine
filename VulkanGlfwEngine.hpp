#pragma once

#include "VulkanHeaderLoadOrder.hpp"
#include "VulkanGlfwDeviceBuilder.hpp"
#include "VulkanLogging.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanPipeline.hpp"

#include "VulkanFramebuffer.hpp"
#include "VulkanCommands.hpp"
#include "VulkanSync.hpp"

#include "GlfwWindow.hpp"
#include "VulkanScene.hpp"

#include "TriangleMesh.hpp"

class VulkanGlfwEngine {
public:
	VulkanGlfwEngine() {};
	VulkanGlfwEngine(int _width, int _height, GLFWwindow *_window, bool _debug);
	
	void initialize();
	void setGlfwWindow(GLFWwindow *_window) { m_window = _window; }
	
	void render(Scene *_scene);

	~VulkanGlfwEngine();
private:

	void recordDrawCommands(
		vk::CommandBuffer _cmdBuffer, 
		uint32_t _imageIndex,
		Scene *_scene);

	bool m_debugMode;
	const char *testExtensions = "VK_EXT_debug_utils";
	const char *testExtensions2 = "VK_KHR_surface";
	//const char *testLayers = "VK_LAYER_KHRONOS_validation";

	// window params
	int m_width;
	int m_height;
	//GlfwWindow m_window = NULL;
	GLFWwindow *m_window;
	

	vk::Instance m_instance = nullptr;
	vk::PhysicalDevice m_physicalDevice = nullptr;
	vkUtils::QueueFamilyIndices m_queueFamilyIndices;
	vk::Device m_logicalDevice = nullptr;
	vk::Queue m_presentQueue = nullptr;
	vk::Queue m_graphicsQueue = nullptr;
	vk::SurfaceKHR m_surface;
	vkSwapchain::SwapchainSupportDetails m_supportDetails;
	
	// swapchain 
	vk::SwapchainKHR m_swapchain;
	std::vector<vkFrame::SwapchainFrame> m_swapchainFrames;
	vk::Format m_swapchainFormat;
	vk::Extent2D m_swapchainExtent;
	void buildSwapchain();
	void rebuildSwapchain();
	void cleanupSwapchain();
	
	// graphics pipeline variables
	vkPipeline::GraphicsPipelineOutBundle m_outputBundle;
	vk::PipelineLayout m_pipelineLayout;
	vk::RenderPass m_renderPass;
	vk::Pipeline m_pipeline;
	void buildFramebuffer();
	int m_maxFramesInFlight; // Max allowd frames to be working on at any given time
	int m_frameNumber; // current frame number

	// Command related variables
	vk::CommandPool m_commandPool;
	vk::CommandBuffer m_mainCommandBuffer;

	// synchronization
	//vk::Fence m_inFlightFence;
	//vk::Semaphore m_imageAvailable;
	// //
	//vk::Semaphore m_renderFinished;
	//
	void buildFrameSyncObjects();

	// asset pointers
	TriangleMesh m_triangleMesh;
	void buildAssets();
	void prepareScene(vk::CommandBuffer _commandBuffer);

	// debugging variables
	static vk::DebugUtilsMessengerEXT m_debugMessenger;
	static vk::DispatchLoaderDynamic m_dispatcher;


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