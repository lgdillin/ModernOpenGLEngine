#include "VulkanApp.hpp"

VulkanApp::VulkanApp() 
	: m_graphicsEngine(800, 600, m_window, true)
	, m_scene() 
{
	createGlfwWindow();
	m_graphicsEngine.setGlfwWindow(m_window);
	m_graphicsEngine.initialize();

};

VulkanApp::~VulkanApp() {
	glfwTerminate();
}

void VulkanApp::createGlfwWindow() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	m_window = glfwCreateWindow(
		m_width,
		m_height,
		"Vulkan Window",
		nullptr, // use this if you want to make it fullscreen
		nullptr // an extra window to share resources with
	);
}

void VulkanApp::run() {
	while (!glfwWindowShouldClose(m_window)) {
		glfwPollEvents();
		calculateFramerate();
		m_graphicsEngine.render(&m_scene);
	}
}

void VulkanApp::calculateFramerate() {
	glfwGetTime();
	m_currentTime = glfwGetTimerValue();
	uint64_t delta = m_currentTime - m_lastTime;
	double currentSeconds = double(m_currentTime) / double(10000000);
	double lastSeconds= double(m_lastTime) / double(10000000);
	double deltaS = currentSeconds - lastSeconds;

	if (deltaS >= 1) {
		int frameRate{ std::max(1, int(m_numFrames / deltaS)) };

		std::stringstream title;
		title << "FPS: " << frameRate;
		glfwSetWindowTitle(m_window, title.str().c_str());
		m_lastTime = m_currentTime;
		m_numFrames = -1;
		//m_frameTime = float(1000.0 / frameRate);
	}
	++m_numFrames;
}
