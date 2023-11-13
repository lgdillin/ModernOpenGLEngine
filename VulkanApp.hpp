#pragma once

#include "VulkanHeaderLoadOrder.hpp"

#include "VulkanGlfwEngine.hpp"
#include "VulkanScene.hpp"

class VulkanApp {
public:
	VulkanApp(); // initializer list for members

	~VulkanApp();

	void createGlfwWindow();

	void run();
	void calculateFramerate();
private:
	VulkanGlfwEngine m_graphicsEngine;

	int m_width = 800;
	int m_height = 600;
	GLFWwindow *m_window;

	Scene m_scene;

	uint64_t m_currentTime, m_lastTime;
	//double m_lastTime, m_currentTime;
	int m_numFrames;
	float m_frameTime;
};