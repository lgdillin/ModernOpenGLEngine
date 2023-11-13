#pragma once

#include "VulkanHeaderLoadOrder.hpp"

#include "VulkanLogging.hpp"
#include "VulkanUtilities.hpp"
#include "VulkanFrame.hpp"

namespace vkSwapchain {
	struct SwapchainSupportDetails {
		vk::SurfaceCapabilitiesKHR capabilities; // single, double buffer
		std::vector<vk::SurfaceFormatKHR> pixelFormats; // supported pixel formats
		std::vector<vk::PresentModeKHR> presentModes; // algorithm for selecting images to present
	};

	struct SwapchainBundle {
		vk::SwapchainKHR swapchain;
		//std::vector<vk::Image> images;
		std::vector<vkFrame::SwapchainFrame> frames;
		vk::Format format;
		vk::Extent2D extent;
	};

	/////////////////////////////////////////////////////////
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

	//////////////////////////////////////////////////////
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

	///////////////////////////////////////////
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

	///////////////////////////////////////////////////////////
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

		vkUtils::QueueFamilyIndices indices
			= vkUtils::findQueueFamilies(_debug, _physicalDevice, _surface);
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

		std::vector<vk::Image> images 
			= _logicalDevice.getSwapchainImagesKHR(bundle.swapchain);
		
		// loop through the images and set image appropiately
		bundle.frames.resize(images.size());
		for (size_t i = 0; i < images.size(); ++i) {
			vk::ImageViewCreateInfo createInfo = {};
			createInfo.image = images[i];
			createInfo.viewType = vk::ImageViewType::e2D;
			createInfo.format = format.format;

			// we can cast it into the B or G channel, but for now we pass
			createInfo.components.r = vk::ComponentSwizzle::eIdentity;
			createInfo.components.g = vk::ComponentSwizzle::eIdentity;
			createInfo.components.b = vk::ComponentSwizzle::eIdentity;
			createInfo.components.a = vk::ComponentSwizzle::eIdentity;
			createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			bundle.frames[i].image = images[i];
			bundle.frames[i].imageView = _logicalDevice.createImageView(createInfo);
		}

		bundle.format = format.format;
		bundle.extent = extent;

		return bundle;
	}
}