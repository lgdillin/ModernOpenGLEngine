#pragma once

#include "VulkanHeaderLoadOrder.hpp"

namespace vkSync {
	static vk::Semaphore createSemaphore(
		vk::Device _logicalDevice,
		bool _debug
	) {
		vk::SemaphoreCreateInfo createInfo = {};
		createInfo.flags = vk::SemaphoreCreateFlags();

		try {
			return _logicalDevice.createSemaphore(createInfo);
		} catch (vk::SystemError err) {
			if (_debug) { std::cout << "failed to create semaphore\n"; }
			return nullptr;
		}
	}

	static vk::Fence createFence(
		vk::Device _logicalDevice,
		bool _debug
	) {
		vk::FenceCreateInfo createInfo = {};
		createInfo.flags = vk::FenceCreateFlags()
			| vk::FenceCreateFlagBits::eSignaled;

		try {
			return _logicalDevice.createFence(createInfo);
		} catch (vk::SystemError err) {
			if (_debug) { std::cout << "Failed to create fence\n"; }
			return nullptr;
		}
	}
}