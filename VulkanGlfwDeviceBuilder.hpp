#pragma once

#include "VulkanHeaderLoadOrder.hpp"

#include "VulkanLogging.hpp"


namespace vkGlfw {
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
		bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
	};

	struct SwapchainSupportDetails {
		vk::SurfaceCapabilitiesKHR capabilities; // single, double buffer
		std::vector<vk::SurfaceFormatKHR> pixelFormats; // supported pixel formats
		std::vector<vk::PresentModeKHR> presentModes; // algorithm for selecting images to present
	};

	struct SwapchainBundle {
		vk::SwapchainKHR swapchain;
		std::vector<vk::Image> images;
		vk::Format format;
		vk::Extent2D extent;
	};

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
				std::cout << "Device details:\n";
				std::string type;
				switch (props.deviceType) {
				case(vk::PhysicalDeviceType::eCpu):
					type = "CPU"; break;
				case(vk::PhysicalDeviceType::eDiscreteGpu):
					type = "Discrete GPU"; break;
				case(vk::PhysicalDeviceType::eIntegratedGpu):
					type = "Integrated GPU"; break;
				case(vk::PhysicalDeviceType::eVirtualGpu):
					type = "Virtual GPU"; break;
				default:
					type = "Other"; break;
				}
				std::cout << "\tDevice Type: " << type << "\n"
					<< "\t" << d.getProperties().deviceName << "\n"
					<< "\t" << d.getProperties().deviceID << "\n"
					<< "\t" << d.getProperties().driverVersion << "\n";
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
		QueueFamilyIndices _indices
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

	static QueueFamilyIndices findQueueFamilies(
		bool _debug, 
		vk::PhysicalDevice _physicalDevice,
		vk::SurfaceKHR _surface	
	) {
		std::vector<vk::QueueFamilyProperties> queueFamilies
			= _physicalDevice.getQueueFamilyProperties();

		if (_debug) { std::cout << queueFamilies.size() << " Queue Families supported\n"; }

		int i = 0;
		QueueFamilyIndices queueFamilyIndices = {};
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

	/////////////////////////////////////////////////////
	static SwapchainSupportDetails querySwapchainSupport(
		bool _debug, 
		vk::PhysicalDevice _physicalDevice,
		vk::SurfaceKHR _surface
	) {
		SwapchainSupportDetails supportDetails = {};
		supportDetails.capabilities = _physicalDevice.getSurfaceCapabilitiesKHR(_surface);

		std::cout << "\tmin img count " << supportDetails.capabilities.minImageCount << "\n";
		std::cout << "\tmax img count " << supportDetails.capabilities.maxImageCount << "\n";

		std::cout << "mins and maxes\n";
		std::cout << "\twidth " << supportDetails.capabilities.minImageExtent.width << "\n";
		std::cout << "\theight " << supportDetails.capabilities.minImageExtent.height << "\n";

		std::cout << "\twidth " << supportDetails.capabilities.maxImageExtent.width << "\n";
		std::cout << "\theight " << supportDetails.capabilities.maxImageExtent.height << "\n";

		std::cout << "\tmax array layer " << supportDetails.capabilities.maxImageArrayLayers << "\n";

		std::cout << "supported transforms\n";
		std::vector<std::string> strings = vkLogging::logTransformBits(
			supportDetails.capabilities.supportedTransforms);
		for (std::string s : strings) {
			std::cout << "\t" << s << "\n";
		}

		std::cout << "current transforms\n";
		strings = vkLogging::logTransformBits(
			supportDetails.capabilities.supportedTransforms);
		for (std::string s : strings) {
			std::cout << "\t" << s << "\n";
		}

		std::cout << "bitmasks\n";
		strings = vkLogging::logAlphaCompositeBits(
			supportDetails.capabilities.supportedCompositeAlpha);
		for (std::string s : strings) {
			std::cout << "\t" << s << "\n";
		}

		std::cout << "image\n";
		strings = vkLogging::logImageUsageBits(
			supportDetails.capabilities.supportedUsageFlags);
		for (std::string s : strings) {
			std::cout << "\t" << s << "\n";
		}

		supportDetails.pixelFormats = _physicalDevice.getSurfaceFormatsKHR(_surface);
		if (_debug) {
			for (vk::SurfaceFormatKHR sf : supportDetails.pixelFormats) {
				std::cout << "supported pixel format: "
					<< vk::to_string(sf.format) << "\n";
				std::cout << "supported color space"
					<< vk::to_string(sf.colorSpace) << "\n";
			}
		}

		supportDetails.presentModes = _physicalDevice.getSurfacePresentModesKHR(_surface);
		for (vk::PresentModeKHR pm : supportDetails.presentModes) {
			std::cout << "\t" << vkLogging::logPresentMode(pm);
		}

		return supportDetails;
	}

	static vk::SurfaceFormatKHR chooseSwapchainSurfaceFormat(
		std::vector<vk::SurfaceFormatKHR> _formats
	) {
		for (vk::SurfaceFormatKHR f : _formats) {
			if (f.format == vk::Format::eB8G8R8A8Unorm
				&& f.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear
			) {
				return f;
			}
		}

		return _formats[0];
	}

	static vk::PresentModeKHR chooseSwapchainPresentMode(
		std::vector<vk::PresentModeKHR> _presentModes
	) {
		for (vk::PresentModeKHR pm : _presentModes) {
			if (pm == vk::PresentModeKHR::eMailbox) {
				return pm;
			}
		}

		return vk::PresentModeKHR::eFifo;
	}

	static vk::Extent2D chooseSwapchainExtent(
		uint32_t _width,
		uint32_t _height,
		vk::SurfaceCapabilitiesKHR _capabilities
	) {
		if (_capabilities.currentExtent.width != UINT32_MAX) {
			return _capabilities.currentExtent;
		} else {
			vk::Extent2D extent = { _width, _height };

			extent.width = std::min(
				_capabilities.maxImageExtent.width,
				std::max(_capabilities.minImageExtent.width, _width)
			);

			extent.height = std::min(
				_capabilities.maxImageExtent.height,
				std::max(_capabilities.minImageExtent.height, _height)
			);

			return extent;
		}
	}

	static SwapchainBundle createSwapchain(
		vk::Device _logicalDevice,
		vk::PhysicalDevice _physicalDevice,
		vk::SurfaceKHR _surface,
		int _width,
		int _height,
		bool _debug
	) {
		SwapchainSupportDetails support
			= querySwapchainSupport(_debug, _physicalDevice, _surface);

		vk::SurfaceFormatKHR format
			= chooseSwapchainSurfaceFormat(support.pixelFormats);

		vk::PresentModeKHR presentMode
			= chooseSwapchainPresentMode(support.presentModes);

		vk::Extent2D extent
			= chooseSwapchainExtent(_width, _height, support.capabilities);

		uint32_t imageCount = std::min(
			support.capabilities.maxImageCount,
			support.capabilities.minImageCount + 1
		);

		// create swapchain create info
		vk::SwapchainCreateInfoKHR createInfo = vk::SwapchainCreateInfoKHR(
			vk::SwapchainCreateFlagsKHR(),
			_surface,
			imageCount, // min image count
			format.format, // image format
			format.colorSpace, // image colorspace
			extent,
			1, // image array layers
			vk::ImageUsageFlagBits::eColorAttachment
		);

		QueueFamilyIndices indices
			= findQueueFamilies(_debug, _physicalDevice, _surface);
		uint32_t queueFamilyIndices[] = {
			indices.graphicsFamily.value(),
			indices.presentFamily.value()
		};

		if (indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		} else {
			createInfo.imageSharingMode = vk::SharingMode::eExclusive;
		}

		createInfo.preTransform = support.capabilities.currentTransform;
		createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;  // if a triangle is partly offscreen, clip

		createInfo.oldSwapchain = vk::SwapchainKHR(nullptr);

		SwapchainBundle bundle = {};
		// might need try catch?
		bundle.swapchain = _logicalDevice.createSwapchainKHR(createInfo);
		//

		bundle.images = _logicalDevice.getSwapchainImagesKHR(bundle.swapchain);
		bundle.format = format.format;
		bundle.extent = extent;

		return bundle;
	}
}