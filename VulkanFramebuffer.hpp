#pragma once

#include "VulkanHeaderLoadOrder.hpp"
#include "VulkanSwapchain.hpp"

namespace vkFramebuffer {

	struct FramebufferInput {
		vk::Device logicalDevice;
		vk::RenderPass renderPass;
		vk::Extent2D swapchainExtent;
	};

	static void createFramebuffers(
		FramebufferInput _inputChunk,
		std::vector<vkFrame::SwapchainFrame> &_frames,
		bool _debug
	) {
		for (int i = 0; i < _frames.size(); ++i) {
			std::vector<vk::ImageView> attachments = {
				_frames[i].imageView
			};

			// information to specify to create a framebuffer
			vk::FramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.flags = vk::FramebufferCreateFlags();
			framebufferInfo.renderPass = _inputChunk.renderPass;
			framebufferInfo.attachmentCount = attachments.size();
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = _inputChunk.swapchainExtent.width;
			framebufferInfo.height = _inputChunk.swapchainExtent.height;
			framebufferInfo.layers = 1;

			try {
				if (_debug) { std::cout << "creating framebuffer for frame " << i << std::endl; }
				_frames[i].framebuffer = _inputChunk.logicalDevice
					.createFramebuffer(framebufferInfo);
			} catch (vk::SystemError err) {
				if (_debug) { std::cout << "Failed to create Framebuffer for frame: " << i << std::endl; }
			}
		}
	}
}