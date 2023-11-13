#pragma once

#include "VulkanHeaderLoadOrder.hpp"

namespace vkUtils {
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
		bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
	};

	static QueueFamilyIndices findQueueFamilies(
		bool _debug,
		vk::PhysicalDevice _physicalDevice,
		vk::SurfaceKHR _surface
	) {
		std::vector<vk::QueueFamilyProperties> queueFamilies
			= _physicalDevice.getQueueFamilyProperties();

		if (_debug) { std::cout << queueFamilies.size() << " Queue Families supported\n"; }

		int i = 0;
		vkUtils::QueueFamilyIndices queueFamilyIndices = {};
		for (vk::QueueFamilyProperties &qfp : queueFamilies) {
			if (qfp.queueFlags & vk::QueueFlagBits::eGraphics) {
				queueFamilyIndices.graphicsFamily = i;
				queueFamilyIndices.presentFamily = i;
				if (_debug) { std::cout << i << "suitable for graphics\n"; }
			}

			if (_physicalDevice.getSurfaceSupportKHR(i, _surface)) {
				//if(false) { // need to fix this
				queueFamilyIndices.presentFamily = i;
				if (_debug) { std::cout << i << "is suitable for presenting\n"; }
			}

			if (queueFamilyIndices.isComplete()) { break; }
			++i;
		}

		return queueFamilyIndices;
	}
}