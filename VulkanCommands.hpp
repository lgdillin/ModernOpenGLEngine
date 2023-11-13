#pragma once

#include "VulkanHeaderLoadOrder.hpp"
#include "VulkanUtilities.hpp"

namespace vkCommands {

	struct CommandBufferInputChunk {
		vk::Device logicalDevice;
		vk::CommandPool commandPool;
		std::vector<vkFrame::SwapchainFrame> &frames;
	};

	static vk::CommandPool createCommandPool(
		vk::Device _logicalDevice,
		vk::PhysicalDevice _physicalDevice,
		vk::SurfaceKHR _surface,
		bool _debug
	) {
		vkUtils::QueueFamilyIndices qIndices
			= vkUtils::findQueueFamilies(_debug, _physicalDevice, _surface);

		vk::CommandPoolCreateInfo poolInfo = {};
		poolInfo.flags = vk::CommandPoolCreateFlags()
			| vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
		poolInfo.queueFamilyIndex = qIndices.graphicsFamily.value();

		try {
			return _logicalDevice.createCommandPool(poolInfo);
		} catch (vk::SystemError err) {
			if (_debug) { std::cout << "Failed to create command pool\n"; }
			return nullptr;
		}
	}


	static vk::CommandBuffer createCommandBuffer(
		CommandBufferInputChunk _inputChunk,
		bool _debug
	) {
		vk::CommandBufferAllocateInfo allocInfo = {};
		allocInfo.commandPool = _inputChunk.commandPool;
		allocInfo.level = vk::CommandBufferLevel::ePrimary;
		allocInfo.commandBufferCount = 1; // create one bufer

		// Create the "main" command buffer
		try {
			vk::CommandBuffer commandBuffer
				= _inputChunk.logicalDevice.allocateCommandBuffers(allocInfo)[0];
			if (_debug) { std::cout << "allocated main cmd buffer\n"; }
			return commandBuffer;
		} catch (vk::SystemError err) {
			if (_debug) { std::cout << "failed to alloc main cmd buffer\n"; }
			return nullptr;
		}
	}

	static void createFrameCommandBuffers(
		CommandBufferInputChunk _inputChunk,
		bool _debug
	) {
		vk::CommandBufferAllocateInfo allocInfo = {};
		allocInfo.commandPool = _inputChunk.commandPool;
		allocInfo.level = vk::CommandBufferLevel::ePrimary;
		allocInfo.commandBufferCount = 1; // create one bufer

		for (int i = 0; i < _inputChunk.frames.size(); ++i) {
			try {
				// select the first one since we are only creating one
				_inputChunk.frames[i].commandBuffer
					= _inputChunk.logicalDevice.allocateCommandBuffers(allocInfo)[0];
				if (_debug) { std::cout << "Allocated cmd buffer for frame " << i << std::endl; }
			} catch (vk::SystemError err) {
				if (_debug) { std::cout << "failed cmd alloc for " << i << std::endl; }
			}
		}
	}
}