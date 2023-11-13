#pragma once

#include "VulkanHeaderLoadOrder.hpp"

namespace vkFrame {
	struct SwapchainFrame {
		vk::Image image;
		vk::ImageView imageView;

		vk::Framebuffer framebuffer;
		vk::CommandBuffer commandBuffer;
		
		vk::Semaphore imageAvailable;
		vk::Semaphore renderFinished;
		vk::Fence inFlight;
	};
}