#include "VulkanGlfwEngine.hpp"

vk::DispatchLoaderDynamic VulkanGlfwEngine::m_dispatcher;
vk::DebugUtilsMessengerEXT VulkanGlfwEngine::m_debugMessenger;

VulkanGlfwEngine::VulkanGlfwEngine(
	int _width,
	int _height,
	GLFWwindow *_window,
	bool _debug
) {
	if (m_debugMode) { std::cout << "Making a graphics engine\n"; }
	m_width = _width;
	m_height = _height;
	m_window = _window;
	m_debugMode = _debug;
}

void VulkanGlfwEngine::initialize() {
	buildInstance();
	buildWindowSurface();

	// construct devices
	m_physicalDevice = vkGlfw::queryPhysicalDevices(m_debugMode, m_instance);
	m_queueFamilyIndices = vkUtils::findQueueFamilies(m_debugMode, m_physicalDevice, m_surface);
	m_logicalDevice = vkGlfw::buildLogicalDevice(m_debugMode, m_physicalDevice, m_queueFamilyIndices);
	
	m_graphicsQueue = m_logicalDevice.getQueue(
		m_queueFamilyIndices.graphicsFamily.value(), 0);

	m_presentQueue = m_logicalDevice.getQueue(
		m_queueFamilyIndices.presentFamily.value(), 0);

	// swapchain
	buildSwapchain();
	m_frameNumber = 0;

	// Graphics pipeline
	vkPipeline::GraphicsPipelineInBundle specs = {};
	specs.logicalDevice = m_logicalDevice;
	specs.vertexFilepath = "vulkan_shaders/VulkanShader1v.spv";
	specs.fragmentFilepath = "vulkan_shaders/VulkanShader1f.spv";
	specs.swapchainImageFormat = m_swapchainFormat;
	specs.screenSize = m_swapchainExtent;
	m_outputBundle = vkPipeline::createGraphicsPipeline(specs, m_debugMode);
	m_pipelineLayout = m_outputBundle.layout;
	m_renderPass = m_outputBundle.renderPass;
	m_pipeline = m_outputBundle.pipeline;

	// framebuffer set up
	buildFramebuffer();

	// command pool set up
	m_commandPool = vkCommands::createCommandPool(m_logicalDevice,
		m_physicalDevice, m_surface, m_debugMode);
	vkCommands::CommandBufferInputChunk inputChunk = {
		m_logicalDevice,
		m_commandPool,
		m_swapchainFrames
	};
	m_mainCommandBuffer = vkCommands::createCommandBuffer(inputChunk, m_debugMode);
	vkCommands::createFrameCommandBuffers(inputChunk, m_debugMode);

	// Synchronization (Fencing, semaphores)
	buildFrameSyncObjects();

	// assets
	buildAssets();

	// debugging
	buildDebugMessenger();

}

///////////////////////////////////////////////////////////////////
void VulkanGlfwEngine::render(Scene *_scene) {
	m_logicalDevice.waitForFences(
		1,
		&m_swapchainFrames[m_frameNumber].inFlight,
		VK_TRUE,
		UINT64_MAX
	);

	uint32_t imageIndex;
	try {
		vk::ResultValue acquire = m_logicalDevice.acquireNextImageKHR(
			m_swapchain,
			UINT64_MAX,
			m_swapchainFrames[m_frameNumber].imageAvailable,
			nullptr
		);
		imageIndex = acquire.value;
	} catch (vk::OutOfDateKHRError) {
		rebuildSwapchain();
		return;
	}


	//uint32_t imageIndex{
	//	m_logicalDevice.acquireNextImageKHR(
	//		m_swapchain,
	//		UINT64_MAX,
	//		m_swapchainFrames[m_frameNumber].imageAvailable,
	//		nullptr).value};

	vk::CommandBuffer commandBuffer = m_swapchainFrames[m_frameNumber].commandBuffer;
	commandBuffer.reset();

	recordDrawCommands(commandBuffer, imageIndex, _scene);

	vk::SubmitInfo submitInfo = {};
	vk::Semaphore waitSemaphores[] = { m_swapchainFrames[m_frameNumber].imageAvailable };
	vk::PipelineStageFlags waitStages[] = {
		// wait just before we output color
		vk::PipelineStageFlagBits::eColorAttachmentOutput
	};

	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;
	vk::Semaphore signalSemaphores[] = { m_swapchainFrames[m_frameNumber].renderFinished };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vk::Result result = m_logicalDevice.resetFences(
		1, &m_swapchainFrames[m_frameNumber].inFlight);
	try {
		m_graphicsQueue.submit(submitInfo, m_swapchainFrames[m_frameNumber].inFlight);
	} catch (vk::SystemError err) {
		if (m_debugMode) { std::cout << "Failed to submit draw command\n"; }
	}

	// present to screen
	vk::PresentInfoKHR presentInfo = {};
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	vk::SwapchainKHR swapchains[] = { m_swapchain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapchains;
	presentInfo.pImageIndices = &imageIndex;

	vk::Result presentResult;
	try {
		presentResult = m_presentQueue.presentKHR(presentInfo);
	} catch (vk::OutOfDateKHRError err) {
		presentResult = vk::Result::eErrorOutOfDateKHR;
	}

	if (presentResult == vk::Result::eErrorOutOfDateKHR
		|| presentResult == vk::Result::eSuboptimalKHR
	) {
		rebuildSwapchain();
		return;
	}

	m_frameNumber = (m_frameNumber + 1) % m_maxFramesInFlight;
}

////////////////////////////////////////////////////////////////
VulkanGlfwEngine::~VulkanGlfwEngine() {
	m_logicalDevice.waitIdle();

	m_logicalDevice.destroyCommandPool(m_commandPool);

	m_logicalDevice.destroyPipeline(m_pipeline);
	m_logicalDevice.destroyPipelineLayout(m_pipelineLayout);
	m_logicalDevice.destroyRenderPass(m_renderPass);

	cleanupSwapchain();

	m_logicalDevice.destroy();
	if (m_debugMode) { std::cout << "logical device Destroyed\n"; }

	m_instance.destroySurfaceKHR(m_surface);
	if (m_debugMode) { std::cout << "surface Destroyed\n"; }

	m_instance.destroyDebugUtilsMessengerEXT(m_debugMessenger, nullptr, m_dispatcher);
	m_instance.destroy();
	if (m_debugMode) { std::cout << "engine Destroyed\n"; }
}

///////////////////////////////////////////////////////
void VulkanGlfwEngine::recordDrawCommands(
	vk::CommandBuffer _cmdBuffer, 
	uint32_t _imageIndex,
	Scene *_scene
) {
	vk::CommandBufferBeginInfo beginInfo = {};
	try {
		_cmdBuffer.begin(beginInfo);
	} catch (vk::SystemError err) {
		if (m_debugMode) { std::cout << "Failed to begin recording command buffer\n"; }
	}


	vk::RenderPassBeginInfo renderInfo = {};
	renderInfo.renderPass = m_renderPass;
	renderInfo.framebuffer = m_swapchainFrames[_imageIndex].framebuffer;
	renderInfo.renderArea.offset.x = 0;
	renderInfo.renderArea.offset.y = 0;
	renderInfo.renderArea.extent = m_swapchainExtent;
	// define clear color for render pass
	vk::ClearValue clearColor = {
		std::array<float, 4>{0.1f, 0.1f, 0.1f, 1.0f} };
	renderInfo.clearValueCount = 1;
	renderInfo.pClearValues = &clearColor;

	_cmdBuffer.beginRenderPass(&renderInfo, vk::SubpassContents::eInline);

	_cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline);

	prepareScene(_cmdBuffer);

	for (glm::vec3 position : _scene->m_trianglePositions) {
		glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
		vkRenderStructs::ObjectData objectData;
		objectData.model = model;
		_cmdBuffer.pushConstants(
			m_pipelineLayout,
			vk::ShaderStageFlagBits::eVertex,
			0,
			sizeof(objectData),
			&objectData
		);

		// hardcoded for now
		_cmdBuffer.draw(
			3, // vertices
			1, // instance
			0, // first vertex
			0 // instance 0
		);
	}

	_cmdBuffer.endRenderPass();
	try {
		_cmdBuffer.end();
	} catch (vk::SystemError err) {
		if (m_debugMode) { std::cout << "failed to stop recording\n"; }
	}
}

//////////////////////////////////////////////////////////
void VulkanGlfwEngine::buildSwapchain() {
	vkSwapchain::SwapchainBundle bundle = vkSwapchain::createSwapchain(
		m_logicalDevice,
		m_physicalDevice,
		m_surface,
		m_width,
		m_height,
		m_debugMode
	);
	m_swapchain = bundle.swapchain;
	m_swapchainFrames = bundle.frames;
	m_swapchainFormat = bundle.format;
	m_swapchainExtent = bundle.extent;
	m_maxFramesInFlight = static_cast<int>(m_swapchainFrames.size());
}

/////////////////////////////////////////////////////////
void VulkanGlfwEngine::rebuildSwapchain() {

	m_width = 0;
	m_height = 0;
	while (m_width == 0 || m_height == 0) {
		glfwGetFramebufferSize(m_window, &m_width, &m_height);
		glfwWaitEvents();
	}

	m_logicalDevice.waitIdle();

	cleanupSwapchain();
	buildSwapchain();
	buildFramebuffer();
	buildFrameSyncObjects();
	vkCommands::CommandBufferInputChunk bufferInput = {
		m_logicalDevice, m_commandPool, m_swapchainFrames };
	m_mainCommandBuffer = vkCommands::createCommandBuffer(bufferInput, m_debugMode);
	vkCommands::createFrameCommandBuffers(bufferInput, m_debugMode);
}

////////////////////////////////////////////
void VulkanGlfwEngine::cleanupSwapchain() {
	for (vkFrame::SwapchainFrame &frame : m_swapchainFrames) {
		m_logicalDevice.destroyFence(frame.inFlight);
		m_logicalDevice.destroySemaphore(frame.imageAvailable);
		m_logicalDevice.destroySemaphore(frame.renderFinished);
		m_logicalDevice.destroyImageView(frame.imageView);
		m_logicalDevice.destroyFramebuffer(frame.framebuffer);
	}
	m_logicalDevice.destroySwapchainKHR(m_swapchain);
}

void VulkanGlfwEngine::buildFramebuffer() {
	vkFramebuffer::FramebufferInput framebufferInput;
	framebufferInput.logicalDevice = m_logicalDevice;
	framebufferInput.renderPass = m_renderPass;
	framebufferInput.swapchainExtent = m_swapchainExtent;
	vkFramebuffer::createFramebuffers(framebufferInput, m_swapchainFrames, m_debugMode);
}

void VulkanGlfwEngine::buildFrameSyncObjects() {
	for (vkFrame::SwapchainFrame &frame : m_swapchainFrames) {
		frame.inFlight = vkSync::createFence(m_logicalDevice, m_debugMode);
		frame.imageAvailable = vkSync::createSemaphore(m_logicalDevice, m_debugMode);
		frame.renderFinished = vkSync::createSemaphore(m_logicalDevice, m_debugMode);
	}
}

void VulkanGlfwEngine::buildAssets() {
	m_triangleMesh = TriangleMesh(m_logicalDevice, m_physicalDevice);
}

void VulkanGlfwEngine::prepareScene(vk::CommandBuffer _commandBuffer) {
	vk::Buffer vertexBuffers[] = { m_triangleMesh.m_vertexBuffer.buffer };
	vk::DeviceSize offsets[] = { 0 };
	_commandBuffer.bindVertexBuffers(
		0,
		1,
		vertexBuffers,
		offsets
	);
}


/////////////////////////////////////////
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

///////////////////////////////////////////////////////////
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
	VkResult result = glfwCreateWindowSurface(m_instance, m_window,
		nullptr, &cStyleSurface);
	if(result == VK_SUCCESS) {
		m_surface = cStyleSurface;
		if (m_debugMode) { std::cout << "success GLFW surface\n"; }
	} else {
		if (m_debugMode) { std::cout << "failed to abstract GLFW surface\n"; }
	}
}
