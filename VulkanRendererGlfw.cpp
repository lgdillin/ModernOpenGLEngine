#include "VulkanRendererGlfw.hpp"

VulkanRendererGlfw::VulkanRendererGlfw() {
	m_glfwWindow = nullptr;
	m_instance = nullptr;
}

VulkanRendererGlfw::~VulkanRendererGlfw() {
	//vkDestroyInstance(m_instance, NULL);
}

int VulkanRendererGlfw::initialize(GlfwWindow *_glfwWindow) {
	m_glfwWindow = _glfwWindow;

	try {
		createInstance();
		createDebugCallback();

		// If the instance supports the surface, create the surface,
		// and make sure that the device supports the surface
		createSurface();
		getPhysicalDevice();
		createLogicalDevice();
	} catch (const std::runtime_error &e) {
		std::cout << "Error: "
			<< e.what() << std::endl;
		return EXIT_FAILURE;
	}
}

void VulkanRendererGlfw::cleanUp() {
	// NULL is for a manual de-allocator
	vkDestroyDevice(m_mainDevice.logicalDevice, nullptr);
	vkDestroyInstance(m_instance, NULL);
	vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

}

void VulkanRendererGlfw::setGlfwWindow(GlfwWindow *_glfwWindow) {

}

void VulkanRendererGlfw::createInstance() {

	// Information about the application itself
	// Most data here doesn't affect the program, and is for
	// developer convenience (like debug info)
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = NULL;
	// custom name of app
	appInfo.pApplicationName = "Vulkan App";
	// version exists as tuple (major, minor, patch)
	appInfo.applicationVersion = 1;
	// custom engine name
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = 1;
	// version of the Vulkan API we plan on using
	appInfo.apiVersion = VK_API_VERSION_1_2;

	// Creation information for a VkInstance
	VkInstanceCreateInfo createInfo = {};
	// first one is structure type
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	// pnext points to extended information for some extension
	createInfo.pNext = NULL;

	// number is modified by bitfields
	createInfo.flags = 0;

	// pointer to a new struct VkApplicationInfo
	createInfo.pApplicationInfo = &appInfo;

	// we have to query these values from vulkan itself
	// list to hold all the extension values we can have

	// create list to hold instance extensions
	std::vector<const char *> instanceExtensions
		= std::vector<const char *>();

	// the extensions acquires to interface with vulkan
	// we need to know what extensions glfw requires to interface
	// with vulkan

	// setup the extensions the instance will use
	// Glfw may require multiple extension
	uint32_t glfwExtensionCount = 0;
	// Extensions are passed as an array of cstrings
	const char **glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	// Add glfw extensions to list of extensions
	for (size_t i = 0; i < glfwExtensionCount; ++i) {
		instanceExtensions.push_back(glfwExtensions[i]);
	}

	if (VALIDATION_ENABLED) {
		instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}

	// check 
	if (!checkInstanceExtensionSupport(&instanceExtensions)) {
		throw std::runtime_error(
			"VkInstance does not support required extensions");
	}

	createInfo.enabledExtensionCount
		= static_cast<uint32_t>(instanceExtensions.size());
	createInfo.ppEnabledExtensionNames = instanceExtensions.data();

	// this is for validation in vulkan
	// add layers to validate certain parts of the program
	// TODO: set up validation layers that instance will use
	createInfo.enabledLayerCount = 0;
	createInfo.ppEnabledLayerNames = nullptr;

	// Create instance
	// When our vulkan creates somehting, it actually tries to allocate
	// something in memory.  we can create a callback function to refer to
	// referenced by pAllocator
	VkResult result = vkCreateInstance(&createInfo, NULL, &m_instance);

	if (result == VK_ERROR_INCOMPATIBLE_DRIVER) {
		std::cout << "cannot find a compatible Vulkan ICD\n";
	} else if (result != VK_SUCCESS) {
		std::cout << result << std::endl;
		throw std::runtime_error("Failed to create a vulkan instance");
	}
}

void VulkanRendererGlfw::createDebugCallback() {
	// Only create callback if validation enabled
	if (!VALIDATION_ENABLED) return;

	VkDebugReportCallbackCreateInfoEXT callbackCreateInfo = {};
	callbackCreateInfo.sType
		= VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;

	// Which validation reports should initiate callback
	callbackCreateInfo.flags =
		VK_DEBUG_REPORT_ERROR_BIT_EXT
		| VK_DEBUG_REPORT_WARNING_BIT_EXT;

	callbackCreateInfo.pfnCallback = debugCallback;												// Pointer to callback function itself

	// Create debug callback with custom create function
	VkResult result = CreateDebugReportCallbackEXT(m_instance,
		&callbackCreateInfo, nullptr, &m_callback);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Debug Callback!");
	}
}

void VulkanRendererGlfw::createLogicalDevice() {
	// Get the queue family inddices for the chosen physical device
	QueueFamilyIndices indices = getQueueFamilies(m_mainDevice.physicalDevice);

	// Vector for queue creation information, and set for family indices
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfoList;
	// since a set can only have one of each type, this is a way of ensuring
	// that we have one and only one unique queue
	std::set<int> queueFamilyIndices = {
		indices.graphicsFamily,
		indices.presentationFamily
	};



	// Queues the logical device needs to create and info to do so
	for (int queueFamilyIndex : queueFamilyIndices) {
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		// The index of the family to create a queue from
		queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
		// number of queues to create
		queueCreateInfo.queueCount = 1;
		// define the priority
		float priority = 1.0f;
		// Vulkan needs to know how to handle multiple queues, so 
		// decide priority (1 = highest priority)
		// priority is [0, 1]
		queueCreateInfo.pQueuePriorities = &priority;

		queueCreateInfoList.push_back(queueCreateInfo);
	}

	// information to create logical device
	// (sometimes called "device")
	// "device" often means logical device, physical device is usually exxplicitly stated
	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	// number of queue create infos
	deviceCreateInfo.queueCreateInfoCount
		= static_cast<uint32_t>(queueCreateInfoList.size());
	// list of queue create infos so devcie can create required queues
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfoList.data();
	// number of enabled logical device extensions
	deviceCreateInfo.enabledExtensionCount = 0;
	// list of enabled logical device extensions
	deviceCreateInfo.ppEnabledExtensionNames = nullptr;
	deviceCreateInfo.enabledLayerCount = 0;

	// physical device features the lgoical device will be using
	VkPhysicalDeviceFeatures deviceFeatures = {};

	// things like geom shader, tess shader, etc.
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

	// Create the logical device for the given physical device
	VkResult result = vkCreateDevice(m_mainDevice.physicalDevice, &deviceCreateInfo,
		nullptr, &m_mainDevice.logicalDevice);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to create a logical device");
	}

	// Queues are created at the same time as the device. so we want
	// handle to queues
	// want to get the queue from the graphicsfamily
	// Fromt given logical device, of given queue family, of given queue
	// index (0 since only one queue), place reference in given vkQueue
	vkGetDeviceQueue(m_mainDevice.logicalDevice, indices.graphicsFamily,
		0, &m_graphicsQueue);

	// get a handle for presentation queue
	vkGetDeviceQueue(m_mainDevice.logicalDevice, indices.presentationFamily,
		0, &m_presentationQueue);
}

void VulkanRendererGlfw::createSurface() {
	// VkWin32SurfaceCreateInfoKHR win32Surface;

	// Creates a vulkan surface for the specified window
	// (Creates a surface create info struct, runs the create
	// surface function, returns result)
	VkResult result = glfwCreateWindowSurface(m_instance,
		m_glfwWindow->getWindow(), nullptr, &m_surface);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to create glfw window surface");
	}
}

void VulkanRendererGlfw::getPhysicalDevice() {
	// Enumerate physical devices the VkInstance can access
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

	// if no devices available, then none support vulkan
	if (deviceCount == 0) {
		throw std::runtime_error("Can't find GPUs that support vulkan instance");
	}

	// Get List of physical devices
	std::vector<VkPhysicalDevice> deviceList(deviceCount);
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, deviceList.data());

	// TEMP: just pick the first device
	//mainDevice.physicalDevice = deviceList[0];

	for (const auto &device : deviceList) {
		if (checkDeviceSuitable(device)) {
			m_mainDevice.physicalDevice = device;
			break;
		}
	}

}

bool VulkanRendererGlfw::checkInstanceExtensionSupport(
	std::vector<const char *> *checkExtensions
) {
	uint32_t extensionCount = 0;

	// Need to get number of extensions to create array of correct
	// size to hold extensions
	vkEnumerateInstanceExtensionProperties(
		nullptr, // check for any extensions at all
		&extensionCount,
		nullptr); // Need to get number of extensions to create

	// create a list of VkExtensionProperties using our count
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr,
		&extensionCount, extensions.data());

	// check if given extensions are in list of available extensions
	for (const auto &checkExtension : *checkExtensions) {
		bool hasExtension = false;
		for (const auto &extension : extensions) {
			std::cout << checkExtension << " " << extension.extensionName << std::endl;
			if (strcmp(checkExtension, extension.extensionName) == 0) {
				hasExtension = true;
				break;
			}
		}

		if (!hasExtension) {
			std::cout << checkExtension << " is not supported" << std::endl;
			return false;
		}
	}

	return true;
}

bool VulkanRendererGlfw::checkDeviceSuitable(VkPhysicalDevice pDevice) {
	//// information about the device itself (ID, name, type, vendor, etc)
	//VkPhysicalDeviceProperties deviceProperties;
	//vkGetPhysicalDeviceProperties(pDevice, &deviceProperties);

	//// information about what the device can do (geo shader, tess shader,
	//// wide lines, etc)
	//VkPhysicalDeviceFeatures deviceFeatures;
	//vkGetPhysicalDeviceFeatures(pDevice, &deviceFeatures);

	QueueFamilyIndices indices = getQueueFamilies(pDevice);

	return indices.isValid();
}

QueueFamilyIndices VulkanRendererGlfw
::getQueueFamilies(VkPhysicalDevice pDevice) {
	QueueFamilyIndices indices;

	// get all queue family property info for the given device
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(pDevice,
		&queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilyList(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(pDevice,
		&queueFamilyCount, queueFamilyList.data());

	// Go through each queue family and check if it has at least
	// one of the required types of queue
	int i = 0;
	for (const auto &queueFamily : queueFamilyList) {

		// First check if queue family has at least 1 queue in that
		// family (could have no queues)
		// Queue can be multiple types defined through bitfield.
		// need to bitwise AND with VK_QUEUE_*_BIT to 
		// check if has required type
		if (queueFamily.queueCount > 0 &&
			queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT
			) {
			// The reason we use flags isntead of hard values is because
			// a queue family can support multiple types of queue

			// if queue family is valid, then get index
			indices.graphicsFamily = i;
		}

		// Check if queue family support presentation
		VkBool32 presentationSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(pDevice, i, m_surface, &presentationSupport);
		// Check if queue is of type presentation
		// (can be both graphics and presentation type)
		if (queueFamily.queueCount > 0 && presentationSupport == true) {
			indices.presentationFamily = i;
		}

		// Check if queue family indices are in a valid state,
		// stop searching if so
		if (indices.isValid()) {
			break;
		}

		++i;
	}

	return indices;
}

SwapChainDetails VulkanRendererGlfw::getSwapChainDetails(VkPhysicalDevice pDevice) {
	return SwapChainDetails();
}
