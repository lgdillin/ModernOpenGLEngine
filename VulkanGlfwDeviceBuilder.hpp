#pragma once

#include "VulkanHeaderLoadOrder.hpp"

#include "VulkanLogging.hpp"
#include "VulkanUtilities.hpp"


namespace vkGlfw {

	static bool checkDeviceSuitability(bool _debug, vk::PhysicalDevice &_device) {
		if (_debug) { std::cout << "Checking device suitability\n"; }

		std::vector<const char *> requestedExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		if (_debug) {
			std::cout << "requesting extensions\n";
			for (const char *re : requestedExtensions) {
				std::cout << "\t" << re << "\n";
			}
		}

		// Check if a given device can satisfy a list of requested extensions
		std::set<std::string> extensionsSet(requestedExtensions.begin(), requestedExtensions.end());

		if (_debug) { std::cout << "device supports extensions:\n"; }
		for (vk::ExtensionProperties &e : _device.enumerateDeviceExtensionProperties()) {
			if (_debug) { std::cout << "\t" << e.extensionName << "\n"; }
			// remove this from the list of required extensions
			// (set checks for equality automatically)
			extensionsSet.erase(e.extensionName);
		}

		if (extensionsSet.empty()) { // all requirements satisfied
			if (_debug) { std::cout << "device can support requested EXT\n"; }
			return true;
		}

		if (_debug) { std::cout << "device cannot support requested EXT\n"; }
		return false;
	}

	static vk::PhysicalDevice queryPhysicalDevices(bool _debug, vk::Instance _instance) {
		if (_debug) { std::cout << "Choosing physical device\n"; }
		vk::PhysicalDevice physicalDevice;

		// query system for available devices
		std::vector<vk::PhysicalDevice> devices = _instance.enumeratePhysicalDevices();
		if (_debug) { std::cout << devices.size() << " devices available\n"; }

		for (vk::PhysicalDevice d : devices) {
			vk::PhysicalDeviceProperties props = d.getProperties();
			if (_debug) {
				vkLogging::logDeviceProperties(d);
			}

			// check if devices is suitable
			if (checkDeviceSuitability(_debug, d)) {
				if (_debug) { std::cout << "Selected\n"; }
				physicalDevice = d;
			}
		}

		return physicalDevice;
	}


	//////////////////////////////////////////
	static vk::Device buildLogicalDevice(bool _debug, vk::PhysicalDevice _physicalDevice,
		vkUtils::QueueFamilyIndices _indices
	) {
		std::vector<uint32_t> uniqueIndices;
		uniqueIndices.push_back(_indices.graphicsFamily.value());

		if (_indices.graphicsFamily.value() != _indices.presentFamily.value()) {
			uniqueIndices.push_back(_indices.presentFamily.value());
		}

		// queue create info
		float queuePriority = 1.0f;
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfo;
		for (uint32_t ui : uniqueIndices) {
			queueCreateInfo.push_back(
				vk::DeviceQueueCreateInfo(
					vk::DeviceQueueCreateFlags(),
					ui,
					1,
					&queuePriority));
		}

		// we need device features
		std::vector<const char *> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
		vk::PhysicalDeviceFeatures deviceFeatures = vk::PhysicalDeviceFeatures();
		//deviceFeatures.samplerAnisotropy = true; // example

		// create the device
		std::vector<const char *> enabledLayers;
		if (_debug) {
			enabledLayers.push_back("VK_LAYER_KHRONOS_validation");
		}

		vk::DeviceCreateInfo deviceInfo = vk::DeviceCreateInfo(
			vk::DeviceCreateFlags(),
			queueCreateInfo.size(), // create one queue
			queueCreateInfo.data(), // creation info
			enabledLayers.size(),
			enabledLayers.data(),
			deviceExtensions.size(), // No extensions
			deviceExtensions.data(), // no extension data
			&deviceFeatures
		);

		vk::Device logicalDevice = _physicalDevice.createDevice(deviceInfo);
		if (_debug) { std::cout << "GPU successfully abstracted\n"; }
		return logicalDevice;
	}
}