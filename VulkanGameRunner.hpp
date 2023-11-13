#pragma once

#include "VulkanHeaderLoadOrder.hpp"

#include "VulkanGlfwEngine.hpp"
#include "GlfwWindow.hpp"

// contains the game loop
class VulkanGameRunner {
public:
	VulkanGameRunner();
	~VulkanGameRunner();

	void run();

private:
	// runs 
};