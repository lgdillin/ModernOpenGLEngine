#pragma once

#include "VulkanHeaderLoadOrder.hpp"

namespace vkLogging {

	static void logDeviceProperties(vk::PhysicalDevice _device) {
		vk::PhysicalDeviceProperties props = _device.getProperties();
		
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
			<< "\t" << _device.getProperties().deviceName << "\n"
			<< "\t" << _device.getProperties().deviceID << "\n"
			<< "\t" << _device.getProperties().driverVersion << "\n";
	}

	static std::vector<std::string> logTransformBits(
		vk::SurfaceTransformFlagsKHR _bits
	) {
		std::vector<std::string> results;

		if (_bits & vk::SurfaceTransformFlagBitsKHR::eIdentity) {
			results.push_back("identity");
		}

		if (_bits & vk::SurfaceTransformFlagBitsKHR::eRotate90) {
			results.push_back("rotate 90");
		}
	
		if (_bits & vk::SurfaceTransformFlagBitsKHR::eRotate180) {
			results.push_back("rotate 180");
		}

		if (_bits & vk::SurfaceTransformFlagBitsKHR::eRotate270) {
			results.push_back("rotate 270");
		}
	
		if (_bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirror) {
			results.push_back("horizontal mirror");
		}

		if (_bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate90) {
			results.push_back("horizontal mirror, 90 rotation");
		}

		if (_bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate180) {
			results.push_back("horizontal mirror, 180 rotation");
		}

		if (_bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate270) {
			results.push_back("horizontal mirror, 270 rotation");
		}

		if (_bits & vk::SurfaceTransformFlagBitsKHR::eInherit) {
			results.push_back("inherited");
		}

		return results;
	}

	static std::vector<std::string> logAlphaCompositeBits(
		vk::CompositeAlphaFlagsKHR _bits
	) {
		std::vector<std::string> results;

		if (_bits & vk::CompositeAlphaFlagBitsKHR::eOpaque) {
			results.push_back("opaque (alpha ignored)");
		}
		if (_bits & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied) {
			results.push_back("pre multiplied (alpha expected to already be multiplied in image)");
		}
		if (_bits & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied) {
			results.push_back("post multiplied (alpha will be applied during composition)");
		}
		if (_bits & vk::CompositeAlphaFlagBitsKHR::eInherit) {
			results.push_back("inherited");
		}

		return results;
	}

	static std::vector<std::string> logImageUsageBits(
		vk::ImageUsageFlags _bits
	) {
		std::vector<std::string> results;

		if (_bits & vk::ImageUsageFlagBits::eTransferSrc) {
			results.push_back("transfer src: image can be used as the source of a transfer command.");
		}
		if (_bits & vk::ImageUsageFlagBits::eTransferDst) {
			results.push_back("transfer dst: image can be used as the destination of a transfer command.");
		}
		if (_bits & vk::ImageUsageFlagBits::eSampled) {
			results.push_back("sampled: image can be used to create a VkImageView suitable for occupying a \
VkDescriptorSet slot either of type VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE or \
VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, and be sampled by a shader.");
		}
		if (_bits & vk::ImageUsageFlagBits::eStorage) {
			results.push_back("storage: image can be used to create a VkImageView suitable for occupying a \
VkDescriptorSet slot of type VK_DESCRIPTOR_TYPE_STORAGE_IMAGE.");
		}
		if (_bits & vk::ImageUsageFlagBits::eColorAttachment) {
			results.push_back("color attachment: image can be used to create a VkImageView suitable for use as \
a color or resolve attachment in a VkFramebuffer.");
		}
		if (_bits & vk::ImageUsageFlagBits::eDepthStencilAttachment) {
			results.push_back("depth/stencil attachment: image can be used to create a VkImageView \
suitable for use as a depth/stencil or depth/stencil resolve attachment in a VkFramebuffer.");
		}
		if (_bits & vk::ImageUsageFlagBits::eTransientAttachment) {
			results.push_back("transient attachment: implementations may support using memory allocations \
with the VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT to back an image with this usage. This \
bit can be set for any image that can be used to create a VkImageView suitable for use as \
a color, resolve, depth/stencil, or input attachment.");
		}
		if (_bits & vk::ImageUsageFlagBits::eInputAttachment) {
			results.push_back("input attachment: image can be used to create a VkImageView suitable for \
occupying VkDescriptorSet slot of type VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT; be read from \
a shader as an input attachment; and be used as an input attachment in a framebuffer.");
		}
		if (_bits & vk::ImageUsageFlagBits::eFragmentDensityMapEXT) {
			results.push_back("fragment density map: image can be used to create a VkImageView suitable \
for use as a fragment density map image.");
		}
		if (_bits & vk::ImageUsageFlagBits::eFragmentShadingRateAttachmentKHR) {
			results.push_back("fragment shading rate attachment: image can be used to create a VkImageView \
suitable for use as a fragment shading rate attachment or shading rate image");
		}
		return results;
	}

	static std::string logPresentMode(vk::PresentModeKHR _presentMode) {
		if (_presentMode == vk::PresentModeKHR::eImmediate) {
			return "immediate: the presentation engine does not wait for a vertical blanking period \
to update the current image, meaning this mode may result in visible tearing. No internal \
queuing of presentation requests is needed, as the requests are applied immediately.";
		}
		if (_presentMode == vk::PresentModeKHR::eMailbox) {
			return "mailbox: the presentation engine waits for the next vertical blanking period \
to update the current image. Tearing cannot be observed. An internal single-entry queue is \
used to hold pending presentation requests. If the queue is full when a new presentation \
request is received, the new request replaces the existing entry, and any images associated \
with the prior entry become available for re-use by the application. One request is removed \
from the queue and processed during each vertical blanking period in which the queue is non-empty.";
		}
		if (_presentMode == vk::PresentModeKHR::eFifo) {
			return "fifo: the presentation engine waits for the next vertical blanking \
period to update the current image. Tearing cannot be observed. An internal queue is used to \
hold pending presentation requests. New requests are appended to the end of the queue, and one \
request is removed from the beginning of the queue and processed during each vertical blanking \
period in which the queue is non-empty. This is the only value of _presentMode that is required \
to be supported.";
		}
		if (_presentMode == vk::PresentModeKHR::eFifoRelaxed) {
			return "relaxed fifo: the presentation engine generally waits for the next vertical \
blanking period to update the current image. If a vertical blanking period has already passed \
since the last update of the current image then the presentation engine does not wait for \
another vertical blanking period for the update, meaning this mode may result in visible tearing \
in this case. This mode is useful for reducing visual stutter with an application that will \
mostly present a new image before the next vertical blanking period, but may occasionally be \
late, and present a new image just after the next vertical blanking period. An internal queue \
is used to hold pending presentation requests. New requests are appended to the end of the queue, \
and one request is removed from the beginning of the queue and processed during or after each \
vertical blanking period in which the queue is non-empty.";
		}
		if (_presentMode == vk::PresentModeKHR::eSharedDemandRefresh) {
			return "shared demand refresh: the presentation engine and application have \
concurrent access to a single image, which is referred to as a shared presentable image. \
The presentation engine is only required to update the current image after a new presentation \
request is received. Therefore the application must make a presentation request whenever an \
update is required. However, the presentation engine may update the current image at any point, \
meaning this mode may result in visible tearing.";
		}
		if (_presentMode == vk::PresentModeKHR::eSharedContinuousRefresh) {
			return "shared continuous refresh: the presentation engine and application have \
concurrent access to a single image, which is referred to as a shared presentable image. The \
presentation engine periodically updates the current image on its regular refresh cycle. The \
application is only required to make one initial presentation request, after which the \
presentation engine must update the current image without any need for further presentation \
requests. The application can indicate the image contents have been updated by making a \
presentation request, but this does not guarantee the timing of when it will be updated. \
This mode may result in visible tearing if rendering to the image is not timed correctly.";
		}
		return "none/undefined";
	}
}