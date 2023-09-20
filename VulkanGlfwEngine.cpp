#include "VulkanGlfwEngine.hpp"

vk::DispatchLoaderDynamic VulkanGlfwEngine::m_dispatcher;
vk::DebugUtilsMessengerEXT VulkanGlfwEngine::m_debugMessenger;

VulkanGlfwEngine::VulkanGlfwEngine() {
	if (m_debugMode) { std::cout << "Making a graphics engine\n"; }
	buildWindow();
	buildInstance();
	buildWindowSurface();

	// construct devices
	m_physicalDevice = vkGlfw::queryPhysicalDevices(m_debugMode, m_instance);
	m_queueFamilyIndices = vkGlfw::findQueueFamilies(m_debugMode, m_physicalDevice, m_surface);
	m_logicalDevice = vkGlfw::buildLogicalDevice(m_debugMode, m_physicalDevice, m_queueFamilyIndices);
	
	m_graphicsQueue = m_logicalDevice.getQueue(
		m_queueFamilyIndices.graphicsFamily.value(), 0);

	m_presentQueue = m_logicalDevice.getQueue(
		m_queueFamilyIndices.presentFamily.value(), 0);

	// swapchain
	vkGlfw::SwapchainBundle bundle = vkGlfw::createSwapchain(
		m_logicalDevice,
		m_physicalDevice,
		m_surface,
		m_width,
		m_height,
		m_debugMode
	);
	m_swapchain = bundle.swapchain;
	m_swapchainImages = bundle.images;
	m_swapchainFormat = bundle.format;
	m_swapchainExtent = bundle.extent;


	// debugging
	buildDebugMessenger();
}

VulkanGlfwEngine::~VulkanGlfwEngine() {
	m_logicalDevice.destroySwapchainKHR(m_swapchain);
	m_logicalDevice.destroy();
	if (m_debugMode) { std::cout << "logical device Destroyed\n"; }

	m_instance.destroySurfaceKHR(m_surface);
	if (m_debugMode) { std::cout << "surface Destroyed\n"; }

	m_instance.destroyDebugUtilsMessengerEXT(m_debugMessenger, nullptr, m_dispatcher);
	m_instance.destroy();
	if (m_debugMode) { std::cout << "engine Destroyed\n"; }

	glfwTerminate();
}

void VulkanGlfwEngine::buildWindow() {
	m_window = GlfwWindow(m_width, m_height);
	m_window.initializeVulkan();
}

void VulkanGlfwEngine::buildInstance() {
	// check which version of vulkan 
	uint32_t version = 0;
	vkEnumerateInstanceVersion(&version);

	if (m_debugMode) {
		std::cout << "System support vulkan Variant: " << VK_API_VERSION_VARIANT(version)
			<< ", Major: " << VK_API_VERSION_MAJOR(version)
			<< ", Minor: " << VK_API_VERSION_MINOR(version)
			<< ", Patch: " << VK_API_VERSION_PATCH(version) << "\n";
	}

	// declare version
	version = VK_MAKE_API_VERSION(0, 1, 1, 0);

	// declare application info
	vk::ApplicationInfo appInfo = vk::ApplicationInfo(
		"Application Name", 
		version, // app version
		"Engine Name", 
		version, // engine version 
		version // api version
	);

	// query which extensions glfw needs in order to interface with vulkan
	uint32_t glfwExtensionsCount = 0;
	const char **glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
	std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionsCount);
	std::vector<const char *> layers;

	if (m_debugMode) {
		layers.push_back("VK_LAYER_KHRONOS_validation");
		extensions.push_back(testExtensions);
		extensions.push_back(testExtensions2);

		std::cout << "extensions to be requested:\n";
		for (const char *extensionName : extensions) {
			std::cout << "\t" << extensionName << "\n";
		}
	} 

	if (!checkExtensionAndLayerSupport(extensions, layers)) {
		std::cout << "not supported\n";
		return;
	}

	// info used to create a vulkan instance
	vk::InstanceCreateInfo createInfo = vk::InstanceCreateInfo(
		vk::InstanceCreateFlags(),
		&appInfo,
		static_cast<uint32_t>(layers.size()),
		layers.data(), // enabled layers
		static_cast<uint32_t>(extensions.size()), 
		extensions.data() // enabled extensions
	);

	m_instance = vk::createInstance(createInfo, nullptr);

	m_dispatcher = vk::DispatchLoaderDynamic(m_instance, vkGetInstanceProcAddr);
}

void VulkanGlfwEngine::buildDebugMessenger() {
	vk::DebugUtilsMessengerCreateInfoEXT createInfo = vk::DebugUtilsMessengerCreateInfoEXT(
		vk::DebugUtilsMessengerCreateFlagsEXT(),
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
		| vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
		| vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
		vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
		| vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
		| vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
		debugCallback,
		nullptr
	);
	
	m_debugMessenger = m_instance.createDebugUtilsMessengerEXT(createInfo, nullptr, m_dispatcher);

}


bool VulkanGlfwEngine::checkExtensionAndLayerSupport(
	std::vector<const char *> &_extensions, 
	std::vector<const char *> &_layers
) {

	std::vector<vk::ExtensionProperties> supportedExtensions
		= vk::enumerateInstanceExtensionProperties();

	// list supported extensions
	if (m_debugMode) {
		std::cout << "Device supports following extensions:\n";
		for (vk::ExtensionProperties ep : supportedExtensions) {
			std::cout << "\t" << ep.extensionName << "\n";
		}
	}
	
	// Check for supported extensions
	bool found;
	for (const char *e : _extensions) {
		found = false;
		for (vk::ExtensionProperties ep : supportedExtensions) {
			if (strcmp(e, ep.extensionName) == 0) {
				found = true;
				if (m_debugMode) { std::cout << e << " is supported\n"; }
			}
		}

		if (!found) {
			if (m_debugMode) { std::cout << e << " not supported\n"; }
			return false;
		}
	}

	// Check for supported layers
	std::vector<vk::LayerProperties> supportedLayers = vk::enumerateInstanceLayerProperties();
	if (m_debugMode) {
		std::cout << "Device supports following layers:\n";
		for (vk::LayerProperties lp : supportedLayers) {
			std::cout << "\t" << lp.layerName << "\n";
		}
	}

	for (const char *l : _layers) {
		found = false;
		for (vk::LayerProperties lp : supportedLayers) {
			if (strcmp(l, lp.layerName) == 0) {
				found = true;
				if (m_debugMode) { std::cout << l << " is supported\n"; }
			}
		}

		if (!found) {
			if (m_debugMode) { std::cout << l << " not supported\n"; }
			return false;
		}
	}

	return true;
}

void VulkanGlfwEngine::buildWindowSurface() {
	VkSurfaceKHR cStyleSurface;
	VkResult result = glfwCreateWindowSurface(m_instance, m_window.getWindow(),
		nullptr, &cStyleSurface);
	if(result == VK_SUCCESS) {
		m_surface = cStyleSurface;
		if (m_debugMode) { std::cout << "success GLFW surface\n"; }
	} else {
		if (m_debugMode) { std::cout << "failed to abstract GLFW surface\n"; }
	}
}
